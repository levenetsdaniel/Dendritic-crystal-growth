#ifndef CRYSTAL_GRID_H
#define CRYSTAL_GRID_H

#include <cstdint>

class Grid {
protected:
    uint64_t nx, ny;
    double dx, dy;

public:
    Grid(uint64_t nx, uint64_t ny, double dx, double dy);

    uint64_t sizeX() const;
    uint64_t sizeY() const;

    bool inside(uint64_t x, uint64_t y);

    uint64_t index(uint64_t x, uint64_t y) const;
};

#endif //CRYSTAL_GRID_H