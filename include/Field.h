#ifndef CRYSTAL_FIELD_H
#define CRYSTAL_FIELD_H
#include <vector>
#include "Grid.h"
#include "MaterialsParametrs.h"

class Field : public Grid {
protected:
    std::vector<float> data;
    std::vector<float> prev_data;
    Parameters p;

public:
    Field(const Parameters& p, uint64_t nx, uint64_t ny, float initValue);

    float at      (uint64_t x, uint64_t y) const;
    float prev_at (uint64_t x, uint64_t y) const;

    void set (uint64_t x, uint64_t y, float value);
    void fill(float value);
    void swap(const Field& other);

    const std::vector<float>& rawData() const { return data; }
    void loadData(const std::vector<float>& d) { data = d; prev_data = d; }
};

#endif // CRYSTAL_FIELD_H