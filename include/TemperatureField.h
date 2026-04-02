#ifndef CRYSTAL_TEMPERATUREFIELD_H
#define CRYSTAL_TEMPERATUREFIELD_H
#include "Field.h"
#include "PhaseField.h"

class TemperatureField: public Field {
public:
    TemperatureField(uint64_t nx, uint64_t ny, double dx, double dy, double initValue = 273.0);

    void applyBoundaryConditions();

    void updataeTemperatureField(double dt, const PhaseField& p_field);
};


#endif //CRYSTAL_TEMPERATUREFIELD_H