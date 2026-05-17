#include "FieldRender.hpp"
#include "PhaseField.h"
#include "TemperatureField.h"
#include <iostream>

int main() {
    Parameters p;

    PhaseField       field(p, 800, 800, 0.0);
    TemperatureField Tfield(p, 800, 800, p.T_init);
    FieldRenderer    renderer(800, 800, 1);
    field.initializeSeed(400, 400, 5, Tfield);

//     std::cout << "phi center:      " << field.at(300, 300) << std::endl;
//     std::cout << "phi seed edge:   " << field.at(315, 300) << std::endl;
//     std::cout << "T inside seed:   " << Tfield.at(300, 300) << std::endl;
//     std::cout << "T outside seed:  " << Tfield.at(325, 300) << std::endl;


    int frame = 0;
    while (renderer.isOpen()) {
        renderer.handleEvents();

        if (frame % 20 == 0) {
            field.RunDLAStep(Tfield, 2);
        }

        field.UpdateField(p.dt, Tfield);
        Tfield.updataeTemperatureField(p.dt, field);

        if (frame % 10 == 0)
            renderer.render(field);

        if (frame % 500 == 0) {
            std::cout << "frame " << frame
                    << " | phi axis:  " << field.at(320, 300)
                    << " | T front:   " << Tfield.at(320, 300)
                    << " | T center:  " << Tfield.at(300, 300)
                    << std::endl;
        }
        frame++;
    }
    return 0;
}