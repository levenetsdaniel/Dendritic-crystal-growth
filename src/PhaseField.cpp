#include "PhaseField.h"
#include <cmath>

#include "MathFunctions.h"

PhaseField::PhaseField(uint64_t nx, uint64_t ny, double dx, double dy, double initValue):
    Field(nx, ny, dx, dy, initValue) {}

void PhaseField::initializeSeed(uint64_t cx, uint64_t cy, double radius) {
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            double dx = i - cx;
            double dy = j - cy;
            double dist = std::sqrt(dx * dx + dy * dy);

            double s = 0.5 * (1.0 - std::tanh((dist - radius) / 1.5));
            data[index(i, j)] = s;
        }
    }
}

double M,a,W,phi,df;


void PhaseField::UpdateField(double dt) {
    prev_data = data;

    for (uint64_t i = 0; i < nx; i++) {
        for (uint64_t j = 0; j < ny; j++) {
            phi = data[index(i,j)];
            double Lap = Laplasian(phi, prev_data[index(i+1, j)], prev_data[index(i-1, j)], prev_data[index(i, j+1)], prev_data[index(i, j-1)], dx, dy);
            double dphi = M * (pow(a,2) * Lap + 4 * W * phi * (1 - phi) * (phi - 0.5 + 15 * phi * (1 - phi) * df / (2 * W)));
            data[index(i, j)] += dphi * dt;
        }
    }
}

