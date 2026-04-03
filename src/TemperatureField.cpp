#include "TemperatureField.h"
#include "MathFunctions.h"
#include "PhaseField.h"

TemperatureField::TemperatureField(const Parameters& p, uint64_t nx, uint64_t ny, double initValue):
    Field(p, nx, ny, initValue) {}

void TemperatureField::applyBoundaryConditions() {
    if (nx < 2 || ny < 2) return;

    for (int i = 0; i < nx; ++i) {
        data[index(i, 0)] = data[index(i, 1)];
        data[index(i, ny - 1)] = data[index(i, ny - 2)];
    }

    for (int j = 0; j < ny; ++j) {
        data[index(0, j)] = data[index(1, j)];
        data[index(nx - 1, j)] = data[index(nx - 2, j)];
    }
}

// double kappa = 1.0, Cp = 1.0, lambda = 1.0;

void TemperatureField::updataeTemperatureField(double dt, const PhaseField& p_field) {
    prev_data = data;
    for (uint64_t i = 1; i < nx - 1; i++) {
        for (uint64_t j = 1; j < ny - 1; j++) {
            double T = prev_data[index(i,j)];
            double phi = p_field.at(i, j);
            double dphi = phi - p_field.prev_at(i, j);
            double lap = Laplasian(T, prev_data[index(i+1, j)], prev_data[index(i-1, j)], prev_data[index(i, j+1)], prev_data[index(i, j-1)], p.dx, p.dy);

            double latent = p.L * dphi * phi * (1.0 - phi);
            double diffusion = p.D * lap;
            double relax = -0.01 * (T - p.Tm);

            double dT = diffusion + latent + relax;

            data[index(i,j)] += dT * dt;
        }
    }
    applyBoundaryConditions();
}
