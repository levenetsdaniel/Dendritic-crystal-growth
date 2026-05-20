#include "TemperatureField.h"
#include "PhaseField.h"
#include <cmath>
#include <omp.h>

TemperatureField::TemperatureField(const Parameters& par, uint64_t nx, uint64_t ny)
        : Field(par, nx, ny, static_cast<float>(par.u_init())) {}

void TemperatureField::applyBoundaryConditions() {
    const float u_inf = static_cast<float>(p.u_init());
    for (uint64_t i = 0; i < nx; ++i) {
        data[index(i, 0)]      = u_inf;
        data[index(i, ny - 1)] = u_inf;
    }
    for (uint64_t j = 0; j < ny; ++j) {
        data[index(0, j)]      = u_inf;
        data[index(nx - 1, j)] = u_inf;
    }
}


static constexpr float BAND_THRESHOLD = 0.9999f;

void TemperatureField::updataeTemperatureField(double dt, const PhaseField& p_field) {
    prev_data = data;

    const float h        = static_cast<float>(p.dx());
    const float inv_h    = 1.0f / h;
    const float inv_h2   = inv_h * inv_h;
    const float inv_4h   = 0.25f * inv_h;
    const float D_tilde  = static_cast<float>(p.D_tilde());
    const float fdt      = static_cast<float>(dt);
    const float at_coef  = 1.0f / (2.0f * std::sqrt(2.0f));
    constexpr float eps_grad = 1e-8f;

#pragma omp parallel for collapse(2) schedule(static)
    for (uint64_t i = 1; i < nx - 1; ++i) {
        for (uint64_t j = 1; j < ny - 1; ++j) {

            // --- Диффузия: считается везде ---
            const float uC = prev_data[index(i,   j  )];
            const float uE = prev_data[index(i+1, j  )];
            const float uW = prev_data[index(i-1, j  )];
            const float uN = prev_data[index(i,   j+1)];
            const float uS = prev_data[index(i,   j-1)];
            const float lap_u = (uE + uW + uN + uS - 4.0f * uC) * inv_h2;

            // Анти-трапинг
            const float phiC = p_field.prev_at(i, j);
            float source = 0.0f;

            if (std::abs(phiC) < BAND_THRESHOLD) {
                // В полосе считаем полный источниковый член
                const float phiE  = p_field.prev_at(i+1, j  );
                const float phiW  = p_field.prev_at(i-1, j  );
                const float phiN  = p_field.prev_at(i,   j+1);
                const float phiS  = p_field.prev_at(i,   j-1);
                const float phiNE = p_field.prev_at(i+1, j+1);
                const float phiNW = p_field.prev_at(i-1, j+1);
                const float phiSE = p_field.prev_at(i+1, j-1);
                const float phiSW = p_field.prev_at(i-1, j-1);

                // ∂φ/∂t в центре и на соседях (для интерполяции на гранях)
                const float fdt_inv = 1.0f / fdt;
                const float dpC = (p_field.at(i,   j  ) - phiC ) * fdt_inv;
                const float dpE = (p_field.at(i+1, j  ) - phiE ) * fdt_inv;
                const float dpW = (p_field.at(i-1, j  ) - phiW ) * fdt_inv;
                const float dpN = (p_field.at(i,   j+1) - phiN ) * fdt_inv;
                const float dpS = (p_field.at(i,   j-1) - phiS ) * fdt_inv;

                // Анти-трапинговый поток на грани:
                auto j_face = [&](float phi_a, float phi_b,
                                  float dpdt_a, float dpdt_b,
                                  float d_normal, float d_tangent) -> float {
                    const float phi_f  = 0.5f * (phi_a  + phi_b );
                    const float dpdt_f = 0.5f * (dpdt_a + dpdt_b);
                    const float gm = std::sqrt(d_normal*d_normal
                                               + d_tangent*d_tangent) + eps_grad;
                    return at_coef * (1.0f - phi_f*phi_f) * dpdt_f
                           * (d_normal / gm);
                };

                const float jR = j_face(phiC, phiE, dpC, dpE,
                                        (phiE  - phiC) * inv_h,
                                        (phiNE + phiN  - phiSE - phiS) * inv_4h);

                const float jL = j_face(phiW, phiC, dpW, dpC,
                                        (phiC  - phiW) * inv_h,
                                        (phiNW + phiN  - phiSW - phiS) * inv_4h);

                const float jT = j_face(phiC, phiN, dpC, dpN,
                                        (phiN  - phiC) * inv_h,
                                        (phiNE + phiE  - phiNW - phiW) * inv_4h);

                const float jB = j_face(phiS, phiC, dpS, dpC,
                                        (phiC  - phiS) * inv_h,
                                        (phiSE + phiE  - phiSW - phiW) * inv_4h);

                const float div_jat = (jR - jL + jT - jB) * inv_h;
                source = 0.5f * dpC - div_jat;
            }

            data[index(i, j)] = uC + (D_tilde * lap_u + source) * fdt;
        }
    }

    applyBoundaryConditions();
}