#include "FieldRender.h"
#include "MaterialsLibrary.h"
#include "PhaseField.h"
#include "TemperatureField.h"
#include "ConfigLoader.h"
#include "Metrics.h"
#include "Checkpoint.h"
#include "Statistics.h"
#include <iostream>
#include <string>
#include <memory>




int main(int argc, char* argv[]) {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n"
              << "║     Karma-Rappel Phase Field Simulator                    ║\n"
              << "║     Crystal Growth & Solidification Dynamics              ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    std::string configPath = "configs/default.json";
    std::string material_name = "SCN";
    bool save_metrics = false;
    bool resume_flag = false;

    // парсинг это самое скучное что придумало человечество
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--material" && i + 1 < argc) {
            material_name = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            material_name = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--no-metrics") {
            save_metrics = false;
        } else if (arg == "--with-metrics") {
            save_metrics = true;
        } else if (arg == "--resume") {
            resume_flag = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: crystal [OPTIONS] [MATERIAL]\n\n"
                      << "Options:\n"
                      << "  MATERIAL                  Material name as first argument (SCN, SILVER, ICE, etc)\n"
                      << "  -m, --material NAME       Select material explicitly\n"
                      << "  --config PATH             Path to config file\n"
                      << "  --with-metrics            Save metrics and frames\n"
                      << "  --no-metrics              Disable metrics and frame saving (default)\n"
                      << "  --resume                  Resume from latest checkpoint\n"
                      << "  -h, --help                Show this help message\n"
                      << "\nExamples:\n"
                      << "  crystal                          # SCN without metrics\n"
                      << "  crystal ICE                      # Ice without metrics\n"
                      << "  crystal SILVER --with-metrics    # Silver with metrics\n"
                      << "  crystal -m ALUMINUM_OXIDE        # Using -m flag\n"
                      << "  crystal --resume                 # Resume from checkpoint\n";
            return 0;
        } else if (arg[0] != '-' && material_name == "SCN") {
            material_name = arg;
        }
    }

    SimulationConfig cfg = ConfigLoader::load(configPath);

    if (save_metrics == false && cfg.save_metrics == true) {
        save_metrics = cfg.save_metrics;
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


    const uint64_t N = cfg.grid_size;

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

    uint64_t frame = 0;

    std::string ckpt_dir = std::string(PROJECT_ROOT_DIR) + "/checkpoints";
    if (resume_flag) {
        std::string ckpt = Checkpoint::latest(ckpt_dir);
        if (!ckpt.empty()) {
            Checkpoint::load(ckpt, frame, field, Tfield);
            std::cout << "✓ Resumed from checkpoint, frame " << frame << "\n\n";
        } else {
            std::cout << "✗ No checkpoints found in " << ckpt_dir << "\n\n";
        }
    }

    std::cout << "Seed initialized\nStarting visualization...\n\n";

    FieldRenderer renderer(N, N, cfg.render_scale);

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


    std::cout << "╔═══════════════════════════════════════════════════════════╗\n"
              << "║                   SIMULATION RUNNING                      ║\n"
              << "║  Close window to stop                                     ║\n"
              << "╚═══════════════════════════════════════════════════════════╝\n\n";

    while (renderer.isOpen()) {
        renderer.handleEvents();

        double dt = p.dt();
        field.UpdateField(dt, Tfield);
        Tfield.updataeTemperatureField(dt, field);

        if (frame % cfg.interval_render == 0) {
            renderer.render(field, Tfield);
        }

        if (frame % cfg.interval_ostream == 0 && frame > 0) {
            uint64_t cx = N / 2, cy = N / 2;
            std::cout << "Frame " << frame
                      << " | phi(c+30)=" << field.at(cx + 30, cy)
                      << " | phi(c+50)=" << field.at(cx + 50, cy)
                      << " | u(c+20)=" << Tfield.at(cx + 20, cy)
                      << std::endl;
        }

        if (save_metrics && m) {
            m->record(frame, cfg.interval_csv, field, Tfield);
            m->saveImage(frame, cfg.interval_snapshot, renderer);
            if (frame % cfg.interval_checkpoint == 0 && frame > 0)
                Checkpoint::save(ckpt_dir, frame, field, Tfield);
        }

        frame++;
    }

    Statistics stat(field, N / 2, N / 2);
    stat.compute();
    stat.printReport();
    if (save_metrics)
        stat.saveToCsv(std::string(PROJECT_ROOT_DIR) + "/runs/final_statistics.csv");

    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n"
              <<   "║                  SIMULATION COMPLETED                     ║\n"
              <<   "╚═══════════════════════════════════════════════════════════╝\n\n";

    return 0;
}