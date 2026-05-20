#include "Field.h"
#include <algorithm>

Field::Field(const Parameters& p, uint64_t nx, uint64_t ny, float initValue)
        : Grid(nx, ny), p(p),
          data     (nx * ny, initValue),
          prev_data(nx * ny, initValue) {}

float Field::at(uint64_t x, uint64_t y) const      { return data[index(x, y)]; }
float Field::prev_at(uint64_t x, uint64_t y) const  { return prev_data[index(x, y)]; }

void Field::set(uint64_t x, uint64_t y, float value) { data[index(x, y)] = value; }

void Field::fill(float value) {
    std::fill(data.begin(), data.end(), value);
}

void Field::swap(const Field& other) {
    data = other.data;
}