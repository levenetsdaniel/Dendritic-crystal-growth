#include "FieldRender.h"
#include "PhaseField.h"
#include "TemperatureField.h"
#include <iostream>

int main() {
    Parameters p;
    p.eps4       = -0.05;
    p.Delta      = 0.75;
    p.dt_safety  = 0.08;
    p.noise_amp  = 0.0;

    const uint64_t N = 1600;

    PhaseField field(p, N, N);
    TemperatureField Tfield(p, N, N);
    FieldRenderer renderer(N, N, 0.5f);

    field.initializeSeed(N / 2, N / 2, 6.0, Tfield);

    std::cout << "=== Karma-Rappel phase-field for SCN ===\n"
            << "  W0/d0    = " << p.W0_over_d0 << "\n"
            << "  dx/W0    = " << p.dx_over_W0 << "\n"
            << "  lambda   = " << p.lambda() << "\n"
            << "  D~       = " << p.D_tilde() << "\n"
            << "  dt (~)   = " << p.dt() << "\n"
            << "  Delta    = " << p.Delta << "\n"
            << "  tau0     = " << p.tau0() << " s\n"
            << "  W0       = " << p.W0() << " cm\n"
            << std::endl;

    int frame = 0;
    while (renderer.isOpen()) {
        renderer.handleEvents();

        field.UpdateField(p.dt(), Tfield);
        Tfield.updataeTemperatureField(p.dt(), field);

        if (frame % 50 == 0)
            renderer.render(field);

        frame++;
    }
    return 0;
}
