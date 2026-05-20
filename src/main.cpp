#include "FieldRender.h"
#include "MaterialsLibrary.h"
#include "PhaseField.h"
#include "TemperatureField.h"
#include "Metrics.h"
#include <iostream>
#include <string>
#include <memory>




int main(int argc, char* argv[]) {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n"
              << "║     Karma-Rappel Phase Field Simulator                     ║\n"
              << "║     Crystal Growth & Solidification Dynamics               ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    std::string material_name = "SCN";
    bool save_metrics = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--material" && i + 1 < argc) {
            material_name = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            material_name = argv[++i];
        } else if (arg == "--no-metrics") {
            save_metrics = false;
        } else if (arg == "--with-metrics") {
            save_metrics = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: crystal [OPTIONS]\n\n"
                      << "Options:\n"
                      << "  -m, --material NAME       Select material (default: SCN)\n"
                      << "                            Available: SCN, SILVER, ALUMINUM_OXIDE, ICE, SILICON\n"
                      << "  --with-metrics            Save metrics and frames\n"
                      << "  --no-metrics              Disable metrics and frame saving (default)\n"
                      << "  -h, --help                Show this help message\n"
                      << "\nExamples:\n"
                      << "  crystal                          # SCN without metrics\n"
                      << "  crystal -m SILVER                # Silver without metrics\n"
                      << "  crystal --material ICE --with-metrics\n";
            return 0;
        }
    }

    Parameters p;
    try {
        p = MaterialsLibrary::getMaterialByName(material_name);
        std::cout << "✓ Material selected: " << material_name << "\n";
        std::cout << "✓ Metrics saving: " << (save_metrics ? "ENABLED" : "DISABLED") << "\n\n";
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

    if (material_name == "ICE" || material_name == "ice" || material_name == "water") {
        renderer.setColorMode(FieldRenderer::ColorMode::ICE_BLUE);
    } else if (material_name == "SILVER" || material_name == "silver" || material_name == "Ag" ||
               material_name == "SILICON" || material_name == "silicon" || material_name == "Si") {
        renderer.setColorMode(FieldRenderer::ColorMode::FIRE);
    } else if (material_name == "ALUMINUM_OXIDE" || material_name == "al2o3" ||
               material_name == "AL2O3" || material_name == "sapphire") {
        renderer.setColorMode(FieldRenderer::ColorMode::GRAYSCALE);
    } else {
        renderer.setColorMode(FieldRenderer::ColorMode::DUAL);
    }

    std::unique_ptr<Metrics> m;
    if (save_metrics) {
        m = std::make_unique<Metrics>(p);
    }

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
            renderer.render(field, Tfield);
        }

        if (frame % 1000 == 0 && frame > 0) {
            uint64_t cx = N / 2, cy = N / 2;
            std::cout << "Frame " << frame
                      << " | phi(c+30)=" << field.at(cx + 30, cy)
                      << " | phi(c+50)=" << field.at(cx + 50, cy)
                      << " | u(c+20)=" << Tfield.at(cx + 20, cy)
                      << std::endl;
        }

        if (save_metrics && m) {
            m->record(frame, 100, field, Tfield);
            m->saveImage(frame, 500, renderer);
        }

        frame++;
    }

    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n"
              << "║                  SIMULATION COMPLETED                      ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    return 0;
}