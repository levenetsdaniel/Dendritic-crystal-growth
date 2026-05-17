#include "TemperatureField.h"
#include "MathFunctions.h"
#include "PhaseField.h"

TemperatureField::TemperatureField(const Parameters& p, uint64_t nx, uint64_t ny, double initValue):
    Field(p, nx, ny, initValue) {}

void TemperatureField::applyBoundaryConditions() {
    for (uint64_t i = 0; i < nx; ++i) {
        data[index(i, 0)]    = p.T_init;
        data[index(i, ny-1)] = p.T_init;
    }
    for (uint64_t j = 0; j < ny; ++j) {
        data[index(0, j)]    = p.T_init;
        data[index(nx-1, j)] = p.T_init;
    }
}


void TemperatureField::updataeTemperatureField(double dt, const PhaseField& p_field) {
    prev_data = data;
    for (uint64_t i = 1; i < nx - 1; i++) {
        for (uint64_t j = 1; j < ny - 1; j++) {
            double T = prev_data[index(i,j)];
            double dphi = p_field.at(i,j) - p_field.prev_at(i,j);

            double lap = Laplasian(
                T,
                prev_data[index(i+1, j)], prev_data[index(i-1, j)],
                prev_data[index(i, j+1)], prev_data[index(i, j-1)],
                p.dx, p.dy
            );

            double diffusion = p.D * lap;
            double latent    = (p.L / p.Cp) * dphi / dt;

            data[index(i,j)] += (diffusion + latent) * dt;
        }
    }
    applyBoundaryConditions();
}
