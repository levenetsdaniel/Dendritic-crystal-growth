#ifndef CRYSTAL_FIELD_H
#define CRYSTAL_FIELD_H
#include <vector>

#include "Grid.h"
#include "MaterialsParametrs.h"

class Field: public Grid{
protected:
    std::vector<double> data;
    std::vector<double> prev_data;
    Parameters p;

public:
    Field(const Parameters& p, uint64_t nx, uint64_t ny, double initValue);

    double at(uint64_t x, uint64_t y) const;

    double prev_at(uint64_t x, uint64_t y) const;

    void set(uint64_t x, uint64_t y, double value);

    void fill(double value);

    void swap(const Field& other);
};

#endif //CRYSTAL_FIELD_H