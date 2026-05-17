#include "PhaseField.h"
#include <cmath>
#include <random>
#include "MaterialsParametrs.h"
#include "MathFunctions.h"
#include "TemperatureField.h"
#include <chrono>
#include <thread>
#include <iostream>

PhaseField::PhaseField(const Parameters& p, uint64_t nx, uint64_t ny, double initValue)
    : Field(p, nx, ny, initValue) {}

void PhaseField::initializeSeed(uint64_t cx, uint64_t cy, double radius,
                                TemperatureField& field) {
    for (uint64_t i = 0; i < nx; ++i) {
        for (uint64_t j = 0; j < ny; ++j) {
            double dx = static_cast<double>(i) - static_cast<double>(cx);
            double dy = static_cast<double>(j) - static_cast<double>(cy);
            double dist = std::sqrt(dx*dx + dy*dy);
            double arg = (dist - radius) / (p.W * 0.70710678);   // W/√2
            data[index(i, j)] = 1.0 / (1.0 + std::exp(arg));
        }
    }
    prev_data = data;
    for (uint64_t i = 0; i < nx; ++i)
        for (uint64_t j = 0; j < ny; ++j)
            if (data[index(i,j)] > 0.9)
                field.set(i, j, p.Tm);
}

void PhaseField::UpdateField(double dt, TemperatureField& field) {
    prev_data = data;
    const double h = p.dx;

    thread_local std::mt19937 rng(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
        + std::hash<std::thread::id>{}(std::this_thread::get_id())
    );
    thread_local std::uniform_real_distribution<double> rnd(-0.5, 0.5);

    for (uint64_t i = 1; i < nx-1; i++) {
        for (uint64_t j = 1; j < ny-1; j++) {
            double phi  = prev_data[index(i,   j  )];
            double phiE = prev_data[index(i+1, j  )];
            double phiW = prev_data[index(i-1, j  )];
            double phiN = prev_data[index(i,   j+1)];
            double phiS = prev_data[index(i,   j-1)];
            double phiNE = prev_data[index(i+1, j+1)];
            double phiNW = prev_data[index(i-1, j+1)];
            double phiSE = prev_data[index(i+1, j-1)];
            double phiSW = prev_data[index(i-1, j-1)];

            // --- Анизотропия Кобаяши на полуточках ---
            double px_r = (phiE - phi)  / h;
            double py_r = (phiNE + phiN - phiSE - phiS) / (4.0*h);
            double th_r = atan2(py_r, px_r);
            double e_r  = 1.0 + p.eps * cos(p.delta * th_r);
            double ep_r = -p.eps * p.delta * sin(p.delta * th_r);

            double px_l = (phi  - phiW) / h;
            double py_l = (phiNW + phiN - phiSW - phiS) / (4.0*h);
            double th_l = atan2(py_l, px_l);
            double e_l  = 1.0 + p.eps * cos(p.delta * th_l);
            double ep_l = -p.eps * p.delta * sin(p.delta * th_l);

            double py_t = (phiN - phi)  / h;
            double px_t = (phiNE + phiE - phiNW - phiW) / (4.0*h);
            double th_t = atan2(py_t, px_t);
            double e_t  = 1.0 + p.eps * cos(p.delta * th_t);
            double ep_t = -p.eps * p.delta * sin(p.delta * th_t);

            double py_b = (phi  - phiS) / h;
            double px_b = (phiSE + phiE - phiSW - phiW) / (4.0*h);
            double th_b = atan2(py_b, px_b);
            double e_b  = 1.0 + p.eps * cos(p.delta * th_b);
            double ep_b = -p.eps * p.delta * sin(p.delta * th_b);

            double main_term = (e_r*e_r*px_r - e_l*e_l*px_l) / h
                             + (e_t*e_t*py_t - e_b*e_b*py_b) / h;

            double cross_term = (-e_r*ep_r*py_r + e_l*ep_l*py_l) / h
                              + ( e_t*ep_t*px_t - e_b*ep_b*px_b) / h;

            double aniso_term = p.W * p.W * (main_term + cross_term);

            double undercooling = p.Tm - field.at(i, j);
            double m = (p.alpha / M_PI) * atan(p.gamma * undercooling);
            double bulk_term = phi * (1.0 - phi) * (phi - 0.5 + m);

            double noise_val = p.noise * rnd(rng) * phi * (1.0 - phi);

            double dphi = p.M * (aniso_term + bulk_term) + noise_val;

            double new_phi = phi + dphi * dt;
            data[index(i,j)] = std::max(0.0, std::min(1.0, new_phi));
        }
    }
}

void PhaseField::RunDLAStep(const TemperatureField& Tfield, int particles_count) {
    thread_local std::mt19937 rng(
        std::chrono::high_resolution_clock::now().time_since_epoch().count() +
        std::hash<std::thread::id>{}(std::this_thread::get_id())
    );
    std::uniform_int_distribution<int> move_dist(0, 3);
    std::uniform_real_distribution<double> angle_dist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

    uint64_t cx = nx / 2;
    uint64_t cy = ny / 2;

    double max_r = 10.0;
    for (uint64_t i = 10; i < nx - 10; i += 5) {
        for (uint64_t j = 10; j < ny - 10; j += 5) {
            if (data[index(i, j)] > 0.5) {
                double r = std::sqrt((static_cast<double>(i) - cx) * (static_cast<double>(i) - cx) +
                                     (static_cast<double>(j) - cy) * (static_cast<double>(j) - cy));
                if (r > max_r) max_r = r;
            }
        }
    }

    double spawn_r = max_r + 15.0;
    if (spawn_r > static_cast<double>(cx) - 10.0) {
        spawn_r = static_cast<double>(cx) - 10.0;
    }

    for (int p_idx = 0; p_idx < particles_count; ++p_idx) {
        double alpha = angle_dist(rng);
        int px = static_cast<int>(cx + spawn_r * std::cos(alpha));
        int py = static_cast<int>(cy + spawn_r * std::sin(alpha));

        bool stuck = false;
        int steps = 0;
        int max_steps = 10000;

        while (!stuck && steps < max_steps) {
            steps++;

            int dir = move_dist(rng);
            if (dir == 0) py++;
            else if (dir == 1) px++;
            else if (dir == 2) py--;
            else if (dir == 3) px--;

            if (px <= 2 || px >= static_cast<int>(nx) - 3 ||
                py <= 2 || py >= static_cast<int>(ny) - 3) {
                break;
            }

            double current_r = std::sqrt(
                (double)(px - (int)cx) * (px - (int)cx) +
                (double)(py - (int)cy) * (py - (int)cy)
            );
            if (current_r > spawn_r + 30.0) break;

            if (data[index(px + 1, py)] > 0.5 || data[index(px - 1, py)] > 0.5 ||
                data[index(px, py + 1)] > 0.5 || data[index(px, py - 1)] > 0.5) {

                double local_T = Tfield.at(px, py);
                double undercooling = p.Tm - local_T;

                if (undercooling > 0.0) {

                    double stick_chance = std::tanh(p.dla_sensitivity * undercooling);

                    if (prob_dist(rng) < stick_chance) {
                        data[index(px, py)] = 1.0;
                        stuck = true;
                    }
                }
            }
        }
    }
}