#ifndef CRYSTAL_FIELD_H
#define CRYSTAL_FIELD_H
#include <vector>

#include "Grid.h"

class Field: public Grid{
protected:
    std::vector<double> data;
    std::vector<double> prev_data;

public:
    Field(uint64_t nx, uint64_t ny, double dx, double dy, double initValue = 0.0);

    double get(uint64_t x, uint64_t y) const;

    void set(uint64_t x, uint64_t y, double value);

    void fill(double value);

    void swap(const Field& other);
};

#endif //CRYSTAL_FIELD_H