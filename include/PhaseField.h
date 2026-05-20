#ifndef CRYSTAL_PHASEFIELD_H
#define CRYSTAL_PHASEFIELD_H
#include "Field.h"
#include "MaterialsParametrs.h"
#include <vector>

class TemperatureField;

class PhaseField : public Field {
    std::vector<uint32_t> active_cells_;
    std::vector<uint8_t>  band_mask_;

public:
    PhaseField(const Parameters& p, uint64_t nx, uint64_t ny, float initValue = -1.0f);

    void initializeSeed(uint64_t cx, uint64_t cy, double radius, TemperatureField& field);

    void UpdateField(double dt, const TemperatureField& field);
};

#endif // CRYSTAL_PHASEFIELD_H