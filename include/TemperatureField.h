#ifndef CRYSTAL_TEMPERATUREFIELD_H
#define CRYSTAL_TEMPERATUREFIELD_H
#include "Field.h"
#include "MaterialsParametrs.h"

class PhaseField;

class TemperatureField: public Field {
public:
    TemperatureField(const Parameters& p, uint64_t nx, uint64_t ny);

    void applyBoundaryConditions();

    void updataeTemperatureField(double dt, const PhaseField& p_field);
};

#endif //CRYSTAL_TEMPERATUREFIELD_H
