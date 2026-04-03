#include "FieldRender.hpp"
#include "PhaseField.h"
#include "TemperatureField.h"
#include <iostream>

int main() {
    Parameters p;
    PhaseField field(p, 200, 200);
    TemperatureField Tfield(p, 200, 200);
    FieldRenderer renderer(200, 200, 3);

    field.initializeSeed(100, 100, 20, Tfield);

    std::cout << "phi at center: " << field.at(100, 100) << std::endl;
    std::cout << "phi at edge:   " << field.at(0, 0) << std::endl;
    std::cout << "phi at (101,100): " << field.at(101, 100) << std::endl;

    int frame = 0;
    while (renderer.isOpen()) {
        renderer.handleEvents();

        field.UpdateField(p.dt, Tfield);
        Tfield.updataeTemperatureField(p.dt, field);

        renderer.render(field);

        if (frame++ % 100 == 0) {
            std::cout << "frame " << frame << " phi center: " << field.at(100,100) << std::endl;
            std::cout << "frame " << frame << " phi near center: " << field.at(50,100) << std::endl;
            std::cout << "frame " << frame << " T center: " << Tfield.at(100,100) << std::endl;
            std::cout << "frame " << frame << " T near center: " << Tfield.at(50,100) << std::endl;
        }
    }

    return 0;
}
