#include "FieldRender.h"
#include "MaterialsLibrary.h"
#include "PhaseField.h"
#include "TemperatureField.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n"
              << "║     Karma-Rappel Phase Field Simulator                     ║\n"
              << "║     Crystal Growth & Solidification Dynamics               ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    std::string material_name = "SCN";
    if (argc > 1) {
        material_name = argv[1];
    }

    Parameters p;
    try {
        p = MaterialsLibrary::getMaterialByName(material_name);
        std::cout << "✓ Material selected: " << material_name << "\n\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Error: Material '" << material_name << "' not found.\n\n";
        MaterialsLibrary::printAvailableMaterials();
        return 1;
    }

    const uint64_t N = 1600;

    std::cout << "Initializing simulation...\n"
              << "  Grid size: " << N << " × " << N << " nodes\n"
              << "  dx (physical): " << p.dx() * p.W0() * 1e4 << " мкм\n"
              << "  dt (physical): " << p.dt() * p.tau0() * 1e6 << " мкс\n"
              << "  W0: " << p.W0() * 1e4 << " мкм\n"
              << "  Lambda: " << p.lambda() << "\n"
              << "  Delta: " << p.Delta << "\n"
              << std::endl;

    PhaseField field(p, N, N);
    TemperatureField Tfield(p, N, N);
    field.initializeSeed(N / 2, N / 2, 10.0, Tfield);

    std::cout << "Seed initialized\nStarting visualization...\n\n";

    FieldRenderer renderer(N, N, 0.5f);

    uint64_t frame = 0;

    std::cout << "╔═══════════════════════════════════════════════════════════╗\n"
              << "║                   SIMULATION RUNNING                       ║\n"
              << "║  Close window to stop                                      ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    while (renderer.isOpen()) {
        renderer.handleEvents();

        double dt = p.dt();
        field.UpdateField(dt, Tfield);
        Tfield.updataeTemperatureField(dt, field);

        if (frame % 5 == 0) {
            renderer.render(field);
        }

        if (frame % 1000 == 0 && frame > 0) {
            uint64_t cx = N / 2, cy = N / 2;
            std::cout << "Frame " << frame
                      << " | phi(c+30)=" << field.at(cx + 30, cy)
                      << " | phi(c+50)=" << field.at(cx + 50, cy)
                      << " | u(c+20)=" << Tfield.at(cx + 20, cy)
                      << std::endl;
        }

        frame++;
    }

    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n"
              << "║                  SIMULATION COMPLETED                      ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    return 0;
}
