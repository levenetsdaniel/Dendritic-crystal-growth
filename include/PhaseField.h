#ifndef CRYSTAL_PHASEFIELD_H
#define CRYSTAL_PHASEFIELD_H
#include "Field.h"

class PhaseField: public Field {
public:
    PhaseField(uint64_t nx, uint64_t ny, double dx, double dy, double initValue = 0.0);

    void initializeSeed(uint64_t cx, uint64_t cy, double radius);
};

#endif //CRYSTAL_PHASEFIELD_H