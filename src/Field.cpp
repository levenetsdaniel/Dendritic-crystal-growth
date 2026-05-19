#include "Field.h"

Field::Field(const Parameters& p, uint64_t nx, uint64_t ny, double initValue):
    Grid(nx, ny), p(p), data(nx * ny, initValue), prev_data(nx * ny, initValue) {}

double Field::at(uint64_t x, uint64_t y) const {
    return data.at(index(x, y));
}

double Field::prev_at(uint64_t x, uint64_t y) const {
    return prev_data.at(index(x, y));
}

void Field::set(uint64_t x,  uint64_t y, double value) {
    data.at(index(x, y)) = value;
}

void Field::fill(double value) {
    for (uint64_t i = 0; i < data.size(); i++) {
        data[i] = value;
    }
}

void Field::swap(const Field& other) {
    for (uint64_t i = 0; i < data.size(); i++) {
        data.at(i) = other.data.at(i);
    }
}
