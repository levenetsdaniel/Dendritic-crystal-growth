#include "PhaseField.h"
#include <cmath>

#include "MaterialsParametrs.h"
#include "MathFunctions.h"
#include "TemperatureField.h"

PhaseField::PhaseField(const Parameters& p, uint64_t nx, uint64_t ny, double initValue):
    Field(p, nx, ny, initValue) {}

void PhaseField::initializeSeed(uint64_t cx, uint64_t cy, double radius, TemperatureField& field) {
    for (uint64_t i = 0; i < nx; ++i) {
        for (uint64_t j = 0; j < ny; ++j) {

            double dx = static_cast<double>(i) - static_cast<double>(cx);
            double dy = static_cast<double>(j) - static_cast<double>(cy);

            double dist = std::sqrt(dx * dx + dy * dy);

            double s = 0.5 * (1.0 - std::tanh((dist - radius) / 0.05));

            data[index(i, j)] = s;
            if (dist < radius) {
                field.set(i, j, p.Tm);
            }
        }
    }
    prev_data = data;
}

// double M = 1.0, a = 1.0, W = 1.0, phi = 1.0, df = 3.0;


void PhaseField::UpdateField(double dt, TemperatureField& field) {
    prev_data = data;

    for (uint64_t i = 1; i < nx - 1; i++) {
        for (uint64_t j = 1; j < ny - 1; j++) {
            double phi = prev_data[index(i,j)];

            double phix = (prev_data[index(i+1,j)] - prev_data[index(i-1,j)]) / (2*p.dx);
            double phiy = (prev_data[index(i,j+1)] - prev_data[index(i,j-1)]) / (2*p.dy);

            double theta = atan2(phiy, phix + 1e-12);

            double a = 1.0 + p.eps * cos(p.delta * theta);
            double a2 = a * a;

            double lap = Laplasian(phi, prev_data[index(i+1, j)], prev_data[index(i-1, j)], prev_data[index(i, j+1)], prev_data[index(i, j-1)], p.dx, p.dy);

            double T = field.at(i,j);
            double gprime = phi * (1.0 - phi) * (1.0 - 2.0 * phi);
            double driving = -p.L * (T - p.Tm) * gprime;

            double noise = p.noise * ((rand()/double(RAND_MAX)) - 0.5) * phi * (1 - phi);

            double well = phi * (1.0 - phi) * (phi - 0.5);

            double dphi = p.M * (a2 * lap - 4 * p.W * well + driving) + noise;
            data[index(i, j)] += dphi * dt;
        }
    }
}

