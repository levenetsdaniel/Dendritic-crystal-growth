#include "TemperatureField.h"

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
