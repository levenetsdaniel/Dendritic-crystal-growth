#include "ConfigLoader.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

SimulationConfig ConfigLoader::load(
        const std::string& filename)
{
    std::filesystem::path configPath =
            std::filesystem::path(PROJECT_ROOT_DIR)
            / filename;

    std::ifstream file(configPath);

    if (!file.is_open()) {
        throw std::runtime_error(
                "Failed to open config file: "
                + configPath.string());
    }

    nlohmann::json j;
    file >> j;

    SimulationConfig cfg;

    cfg.material =
            j.value("material", "SCN");

    cfg.grid_size =
            j.value("grid_size", 1600);

    cfg.seed_radius =
            j.value("seed_radius", 10.0);

    cfg.render_scale =
            j.value("render_scale", 0.5f);

    cfg.interval_render =
            j.value("interval_render", 5);

    cfg.interval_csv =
            j.value("interval_csv", 100);

    cfg.interval_snapshot =
            j.value("interval_snapshot", 500);

    cfg.interval_ostream =
            j.value("interval_ostream", 1000);

    cfg.interval_checkpoint =
            j.value("interval_checkpoint", 5000);

    cfg.save_metrics =
            j.value("save_metrics", true);

    return cfg;
}