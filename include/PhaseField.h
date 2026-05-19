#ifndef CRYSTAL_PHASEFIELD_H
#define CRYSTAL_PHASEFIELD_H
#include "Field.h"
#include "MaterialsParametrs.h"

class TemperatureField;

class PhaseField: public Field {
public:
    PhaseField(const Parameters& p, uint64_t nx, uint64_t ny, double initValue = -1.0);

    void initializeSeed(uint64_t cx, uint64_t cy, double radius, TemperatureField& field);

    void UpdateField(double dt, const TemperatureField& field);
};

#endif //CRYSTAL_PHASEFIELD_H
