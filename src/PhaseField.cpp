#include "PhaseField.h"
#include "TemperatureField.h"
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <omp.h>

PhaseField::PhaseField(const Parameters& par, uint64_t nx, uint64_t ny, float initValue)
        : Field(par, nx, ny, initValue),
          band_mask_(nx * ny, 0) {}


void PhaseField::initializeSeed(uint64_t cx, uint64_t cy, double radius,
                                TemperatureField& field) {
    for (uint64_t i = 0; i < nx; ++i) {
        for (uint64_t j = 0; j < ny; ++j) {
            double xi   = static_cast<double>(i) - static_cast<double>(cx);
            double yj   = static_cast<double>(j) - static_cast<double>(cy);
            double dist = std::sqrt(xi*xi + yj*yj) * p.dx();
            double r_w0 = radius * p.dx();
            data[index(i, j)] = static_cast<float>(
                    -std::tanh((dist - r_w0) / std::sqrt(2.0)));
        }
    }
    prev_data = data;

    for (uint64_t i = 0; i < nx; ++i)
        for (uint64_t j = 0; j < ny; ++j)
            if (data[index(i, j)] > 0.0f)
                field.set(i, j, 0.0f);
}


static constexpr float BAND_CORE = 0.9999f;

static inline std::pair<float,float> aniso(float dx, float dy, float eps4) {
    const float g2   = dx*dx + dy*dy + 1e-20f;
    const float c2   = (dx*dx - dy*dy) / g2;
    const float s2   = 2.0f * dx * dy  / g2;
    const float cos4 = 2.0f * c2*c2 - 1.0f;
    const float sin4 = 2.0f * s2 * c2;
    return { 1.0f + eps4 * cos4,
             -4.0f * eps4 * sin4 };
}

void PhaseField::UpdateField(double dt, const TemperatureField& field) {
    prev_data = data;

    const float h      = static_cast<float>(p.dx());
    const float inv_h  = 1.0f / h;
    const float inv_2h = 0.5f  * inv_h;
    const float inv_4h = 0.25f * inv_h;
    const float lambda = static_cast<float>(p.lambda());
    const float eps4   = static_cast<float>(p.eps4);
    const float fdt    = static_cast<float>(dt);


    std::fill(band_mask_.begin(), band_mask_.end(), 0u);

    for (uint64_t i = 1; i < nx - 1; ++i)
        for (uint64_t j = 1; j < ny - 1; ++j)
            if (std::abs(prev_data[index(i, j)]) < BAND_CORE)
                band_mask_[index(i, j)] = 1u;

    active_cells_.clear();
    for (uint64_t i = 1; i < nx - 1; ++i) {
        for (uint64_t j = 1; j < ny - 1; ++j) {
            const uint64_t idx = index(i, j);

            if (band_mask_[idx]      ||
                band_mask_[idx + 1]  || band_mask_[idx - 1]  ||
                band_mask_[idx + nx] || band_mask_[idx - nx])
                active_cells_.push_back(static_cast<uint32_t>(idx));
        }
    }

    thread_local std::mt19937 rng(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
            + std::hash<std::thread::id>{}(std::this_thread::get_id()));
    thread_local std::uniform_real_distribution<float> rnd(-0.5f, 0.5f);

    const auto n_active = static_cast<int64_t>(active_cells_.size());

    // Параллелочка во благо оптимизации времени
#pragma omp parallel for schedule(static)
    for (int64_t k = 0; k < n_active; ++k) {
        const uint32_t flat = active_cells_[k];
        const uint64_t i = flat % nx;
        const uint64_t j = flat / nx;

        const float phi   = prev_data[index(i,   j  )];
        const float phiE  = prev_data[index(i+1, j  )];
        const float phiW  = prev_data[index(i-1, j  )];
        const float phiN  = prev_data[index(i,   j+1)];
        const float phiS  = prev_data[index(i,   j-1)];
        const float phiNE = prev_data[index(i+1, j+1)];
        const float phiNW = prev_data[index(i-1, j+1)];
        const float phiSE = prev_data[index(i+1, j-1)];
        const float phiSW = prev_data[index(i-1, j-1)];

        const float dxR = (phiE - phi)                  * inv_h;
        const float dyR = (phiNE + phiN - phiSE - phiS) * inv_4h;
        auto [aR, apR] = aniso(dxR, dyR, eps4);

        const float dxL = (phi  - phiW)                 * inv_h;
        const float dyL = (phiNW + phiN - phiSW - phiS) * inv_4h;
        auto [aL, apL] = aniso(dxL, dyL, eps4);

        const float dyT = (phiN - phi)                  * inv_h;
        const float dxT = (phiNE + phiE - phiNW - phiW) * inv_4h;
        auto [aT, apT] = aniso(dxT, dyT, eps4);

        const float dyB = (phi  - phiS)                 * inv_h;
        const float dxB = (phiSE + phiE - phiSW - phiW) * inv_4h;
        auto [aB, apB] = aniso(dxB, dyB, eps4);

        const float main_term =
                (aR*aR * dxR - aL*aL * dxL) * inv_h
                + (aT*aT * dyT - aB*aB * dyB) * inv_h;

        const float cross_term =
                - (aR * apR * dyR - aL * apL * dyL) * inv_h
                + (aT * apT * dxT - aB * apB * dxB) * inv_h;

        const float dxC = (phiE - phiW) * inv_2h;
        const float dyC = (phiN - phiS) * inv_2h;
        auto [aC, ignore] = aniso(dxC, dyC, eps4);

        const float u  = field.at(i, j);
        const float m  = 1.0f - phi * phi;
        const float double_well = phi - phi * phi * phi;
        const float coupling    = -lambda * u * m * m;

        float noise_val = 0.0f;
        if (p.noise_amp > 0.0) {
            noise_val = static_cast<float>(p.noise_amp) * rnd(rng) * m * m;
        }

        const float rhs     = main_term + cross_term + double_well + coupling + noise_val;
        const float new_phi = phi + (rhs / (aC * aC)) * fdt;
        data[flat] = std::clamp(new_phi, -1.0f, 1.0f);
    }
}