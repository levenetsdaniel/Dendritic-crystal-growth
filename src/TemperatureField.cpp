#include "TemperatureField.h"
#include "PhaseField.h"
#include <cmath>
#include <omp.h>

TemperatureField::TemperatureField(const Parameters& par, uint64_t nx, uint64_t ny):
    Field(par, nx, ny, par.u_init()) {}

void TemperatureField::applyBoundaryConditions() {
    const double u_inf = p.u_init();
    for (uint64_t i = 0; i < nx; ++i) {
        data[index(i, 0)]      = u_inf;
        data[index(i, ny - 1)] = u_inf;
    }
    for (uint64_t j = 0; j < ny; ++j) {
        data[index(0, j)]      = u_inf;
        data[index(nx - 1, j)] = u_inf;
    }
}

void TemperatureField::updataeTemperatureField(double dt, const PhaseField& p_field) {
    prev_data = data;

    const double h        = p.dx();
    const double D_tilde  = p.D_tilde();
    const double at_coef  = 1.0 / (2.0 * std::sqrt(2.0));
    constexpr double eps_grad = 1e-8;   // защита от деления на ноль

    auto dphi_dt = [&](uint64_t i, uint64_t j) {
        return (p_field.at(i, j) - p_field.prev_at(i, j)) / dt;
    };

    auto j_at_face = [&](double phi_a, double phi_b,
                         double dpdt_a, double dpdt_b,
                         double d_normal_phi, double d_tangent_phi) {
        double phi_f  = 0.5 * (phi_a + phi_b);
        double dpdt_f = 0.5 * (dpdt_a + dpdt_b);
        double grad_mag = std::sqrt(d_normal_phi * d_normal_phi
                                  + d_tangent_phi * d_tangent_phi) + eps_grad;
        return at_coef * (1.0 - phi_f * phi_f) * dpdt_f
               * (d_normal_phi / grad_mag);
    };

    #pragma omp parallel for collapse(2) schedule(static)
    for (uint64_t i = 1; i < nx - 1; ++i) {
        for (uint64_t j = 1; j < ny - 1; ++j) {

            double uC = prev_data[index(i,   j  )];
            double uE = prev_data[index(i+1, j  )];
            double uW = prev_data[index(i-1, j  )];
            double uN = prev_data[index(i,   j+1)];
            double uS = prev_data[index(i,   j-1)];
            double lap_u = (uE + uW + uN + uS - 4.0 * uC) / (h * h);

            double phiC  = p_field.prev_at(i,   j  );
            double phiE  = p_field.prev_at(i+1, j  );
            double phiW  = p_field.prev_at(i-1, j  );
            double phiN  = p_field.prev_at(i,   j+1);
            double phiS  = p_field.prev_at(i,   j-1);
            double phiNE = p_field.prev_at(i+1, j+1);
            double phiNW = p_field.prev_at(i-1, j+1);
            double phiSE = p_field.prev_at(i+1, j-1);
            double phiSW = p_field.prev_at(i-1, j-1);

            double dpC = dphi_dt(i,   j  );
            double dpE = dphi_dt(i+1, j  );
            double dpW = dphi_dt(i-1, j  );
            double dpN = dphi_dt(i,   j+1);
            double dpS = dphi_dt(i,   j-1);

            double jR = j_at_face(
                phiC, phiE, dpC, dpE,
                (phiE - phiC) / h,
                (phiNE + phiN - phiSE - phiS) / (4.0 * h)
            );
            double jL = j_at_face(
                phiW, phiC, dpW, dpC,
                (phiC - phiW) / h,
                (phiNW + phiN - phiSW - phiS) / (4.0 * h)
            );
            double jT = j_at_face(
                phiC, phiN, dpC, dpN,
                (phiN - phiC) / h,
                (phiNE + phiE - phiNW - phiW) / (4.0 * h)
            );
            double jB = j_at_face(
                phiS, phiC, dpS, dpC,
                (phiC - phiS) / h,
                (phiSE + phiE - phiSW - phiW) / (4.0 * h)
            );

            double div_jat = (jR - jL) / h + (jT - jB) / h;

            double rhs = D_tilde * lap_u + 0.5 * dpC - div_jat;
            data[index(i, j)] = uC + rhs * dt;
        }
    }

    applyBoundaryConditions();
}
