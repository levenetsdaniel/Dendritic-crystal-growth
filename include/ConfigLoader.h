#ifndef CRYSTAL_CONFIGLOADER_H
#define CRYSTAL_CONFIGLOADER_H

#include <string>
#include <cstdint>

struct SimulationConfig
{
    std::string material;

    uint64_t grid_size;

    double seed_radius;

    float render_scale;

    int interval_render;
    int interval_csv;
    int interval_snapshot;
    int interval_ostream;

    bool save_metrics;
};

class ConfigLoader
{
public:
    static SimulationConfig load(
            const std::string& filename);
};

#endif // CRYSTAL_CONFIGLOADER_H