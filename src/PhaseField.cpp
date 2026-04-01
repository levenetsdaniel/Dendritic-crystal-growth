#include "PhaseField.h"

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
