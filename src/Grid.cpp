#include "Grid.h"

Grid::Grid(uint64_t nx, uint64_t ny, double dx, double dy): nx(nx), ny(ny), dx(dx), dy(dy) {}

uint64_t Grid::sizeX() const {
    return nx;
}

uint64_t Grid::sizeY() const {
    return ny;
}

bool Grid::inside(uint64_t x, uint64_t y) const {
    return x < nx && y < ny;
}

uint64_t Grid::index(uint64_t x, uint64_t y) const {
    return y * nx + x;
}
