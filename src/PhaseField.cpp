#include "PhaseField.h"
#include "TemperatureField.h"
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <omp.h>

PhaseField::PhaseField(const Parameters& par, uint64_t nx, uint64_t ny, double initValue)
    : Field(par, nx, ny, initValue) {}

// ------------------------------------------------------------
//  Зародыш: tanh-профиль ширины ~1 в безразмерных единицах W0.
//  Внутри зародыша задаём u = 0 (температура плавления).
// ------------------------------------------------------------
void PhaseField::initializeSeed(uint64_t cx, uint64_t cy, double radius,
                                TemperatureField& field) {
    for (uint64_t i = 0; i < nx; ++i) {
        for (uint64_t j = 0; j < ny; ++j) {
            double xi  = static_cast<double>(i) - static_cast<double>(cx);
            double yj  = static_cast<double>(j) - static_cast<double>(cy);
            double dist = std::sqrt(xi*xi + yj*yj) * p.dx();      // в единицах W0
            double r_w0 = radius * p.dx();                         // тоже в W0

            // tanh-профиль: phi = +1 в центре, -1 снаружи
            data[index(i, j)] = -std::tanh((dist - r_w0) / std::sqrt(2.0));
        }
    }
    prev_data = data;

    // Прогрев зародыша до температуры плавления (u = 0)
    for (uint64_t i = 0; i < nx; ++i)
        for (uint64_t j = 0; j < ny; ++j)
            if (data[index(i, j)] > 0.0)
                field.set(i, j, 0.0);
}

// ------------------------------------------------------------
//  Один шаг Karma-Rappel для phi.
//
//  В безразмерных единицах (длина / W0, время / tau0):
//
//    a(theta)^2 * d_t phi  =  div[ a^2 grad phi ]
//                           + d_x[ |grad phi|^2 a a' d_y phi ]
//                           - d_y[ |grad phi|^2 a a' d_x phi ]
//                           + phi - phi^3
//                           - lambda * u * (1 - phi^2)^2
//
//  где a(theta) = 1 + eps4 cos(4 theta),
//      a'(theta) = -4 eps4 sin(4 theta),
//      theta = atan2(d_y phi, d_x phi).
//
//  Дискретизация: 9-точечный шаблон, оператор разбит на
//  потоки через грани ячейки (i±1/2, j) и (i, j±1/2).
//  Это стандартная схема для Karma-Rappel.
// ------------------------------------------------------------
void PhaseField::UpdateField(double dt, const TemperatureField& field) {
    prev_data = data;

    const double h      = p.dx();
    const double lambda = p.lambda();
    const double eps4   = p.eps4;

    // Локальная лямбда: считает поток на грани по двум центральным
    // соседям; возвращает (a^2 * dphi/dn) и кросс-член анизотропии.
    // Реализована inline в цикле — выноса в функцию не делаю,
    // чтобы компилятор хорошо векторизовал.

    thread_local std::mt19937 rng(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
        + std::hash<std::thread::id>{}(std::this_thread::get_id())
    );
    thread_local std::uniform_real_distribution<double> rnd(-0.5, 0.5);

    #pragma omp parallel for collapse(2) schedule(static)
    for (uint64_t i = 1; i < nx - 1; ++i) {
        for (uint64_t j = 1; j < ny - 1; ++j) {

            // 9-точечный шаблон
            double phi   = prev_data[index(i,   j  )];
            double phiE  = prev_data[index(i+1, j  )];
            double phiW  = prev_data[index(i-1, j  )];
            double phiN  = prev_data[index(i,   j+1)];
            double phiS  = prev_data[index(i,   j-1)];
            double phiNE = prev_data[index(i+1, j+1)];
            double phiNW = prev_data[index(i-1, j+1)];
            double phiSE = prev_data[index(i+1, j-1)];
            double phiSW = prev_data[index(i-1, j-1)];

            // --- Грань (i+1/2, j): "right" ---
            double dxR = (phiE - phi) / h;
            double dyR = (phiNE + phiN - phiSE - phiS) / (4.0 * h);
            double thR = std::atan2(dyR, dxR);
            double aR  = 1.0 + eps4 * std::cos(4.0 * thR);
            double apR = -4.0 * eps4 * std::sin(4.0 * thR);

            // --- Грань (i-1/2, j): "left" ---
            double dxL = (phi - phiW) / h;
            double dyL = (phiNW + phiN - phiSW - phiS) / (4.0 * h);
            double thL = std::atan2(dyL, dxL);
            double aL  = 1.0 + eps4 * std::cos(4.0 * thL);
            double apL = -4.0 * eps4 * std::sin(4.0 * thL);

            // --- Грань (i, j+1/2): "top" ---
            double dyT = (phiN - phi) / h;
            double dxT = (phiNE + phiE - phiNW - phiW) / (4.0 * h);
            double thT = std::atan2(dyT, dxT);
            double aT  = 1.0 + eps4 * std::cos(4.0 * thT);
            double apT = -4.0 * eps4 * std::sin(4.0 * thT);

            // --- Грань (i, j-1/2): "bottom" ---
            double dyB = (phi - phiS) / h;
            double dxB = (phiSE + phiE - phiSW - phiW) / (4.0 * h);
            double thB = std::atan2(dyB, dxB);
            double aB  = 1.0 + eps4 * std::cos(4.0 * thB);
            double apB = -4.0 * eps4 * std::sin(4.0 * thB);

            // --- Основной (изотропный) член:  div[ a^2 grad phi ] ---
            double main_term =
                  (aR*aR * dxR - aL*aL * dxL) / h
                + (aT*aT * dyT - aB*aB * dyB) / h;

            // --- Кросс-член анизотропии (Karma-Rappel, PRE 57, 1998, ур. 6):
            //     - d_x[ a a' d_y phi ]  +  d_y[ a a' d_x phi ]
            //
            //  Без множителя |grad phi|^2 (это была ошибка из формулировки
            //  Кобаяши, где он возникает из-за иной записи функционала).
            //  На гранях R/L считаем поток по x, на T/B — по y.
            double cross_term =
                - ( aR * apR * dyR  -  aL * apL * dyL ) / h
                + ( aT * apT * dxT  -  aB * apB * dxB ) / h;

            // --- Локальная анизотропия в коэффициенте при d_t phi ---
            // a(theta) в центре — для нормировки tau(theta) = a^2.
            // Считаем по среднему градиенту (центральные разности).
            double dxC = (phiE - phiW) / (2.0 * h);
            double dyC = (phiN - phiS) / (2.0 * h);
            double thC = std::atan2(dyC, dxC);
            double aC  = 1.0 + eps4 * std::cos(4.0 * thC);

            // --- Источник: двойная яма + сцепка с температурой ---
            double u = field.at(i, j);
            double double_well = phi - phi*phi*phi;
            double coupling    = -lambda * u * (1.0 - phi*phi) * (1.0 - phi*phi);

            // --- Тепловой шум на интерфейсе (если включён) ---
            double noise_val = 0.0;
            if (p.noise_amp > 0.0) {
                // Локальный градиент phi (центральные разности)
                double gx = (phiE - phiW) / (2.0 * h);
                double gy = (phiN - phiS) / (2.0 * h);
                double grad_mag_sq = gx*gx + gy*gy;

                // Шум только там, где градиент существенный (т.е. на интерфейсе)
                // В объёме градиент → 0, шум выключен полностью.
                if (grad_mag_sq > 0.01) {  // порог
                    double interface_factor = (1.0 - phi*phi) * (1.0 - phi*phi);
                    noise_val = p.noise_amp * rnd(rng) * interface_factor;
                }
            }

            // --- Собираем dphi/dt и шагаем явным Эйлером ---
            double rhs = main_term + cross_term + double_well + coupling + noise_val;
            double dphi_dt = rhs / (aC * aC);

            double new_phi = phi + dphi_dt * dt;
            // Мягкий clamp на случай численного шума; в норме он не нужен.
            data[index(i, j)] = std::clamp(new_phi, -1.0, 1.0);
        }
    }
}