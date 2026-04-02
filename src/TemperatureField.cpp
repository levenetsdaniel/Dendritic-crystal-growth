#include "TemperatureField.h"
#include "MathFunctions.h"
#include "PhaseField.h"

TemperatureField::TemperatureField(uint64_t nx, uint64_t ny, double dx, double dy, double initValue):
    Field(nx, ny, dx, dy, initValue) {}

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

double kappa, Cp, L;

void TemperatureField::updataeTemperatureField(double dt, const PhaseField& p_field) {
    prev_data = data;
    for (uint64_t i = 0; i < nx; i++) {
        for (uint64_t j = 0; j < ny; j++) {
            double dphi = ( data[index(i,j)] - prev_data[index(i,j)] ) / dt;
            double T = data[index(i,j)];
            double Lap = Laplasian(T, prev_data[index(i+1, j)], prev_data[index(i-1, j)], prev_data[index(i, j+1)], prev_data[index(i, j-1)], dx, dy);
            double dT = kappa * Lap + L / Cp * dphi;
            data[index(i,j)] += dT * dt;
        }
    }
}
