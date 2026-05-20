#include "Metrics.h"
#include <iomanip>
#include <sstream>
#include <filesystem>

Metrics::Metrics(const Parameters& p)
{
    run_id_ =
            "run_eps"
            + std::to_string(p.eps4)
            + "_D"
            + std::to_string(p.Delta)
            + "_"
            + timestamp();

    run_dir_ = std::filesystem::path(PROJECT_ROOT_DIR) / "runs" / run_id_;

    std::filesystem::create_directories(run_dir_);
    std::filesystem::create_directories(run_dir_ / "frames");

    csv_.open((run_dir_ / "metrics.csv").string());

    csv_ << "step,wall_time,area,radius,T_interface\n";

    startTime_ = std::chrono::steady_clock::now();
}

std::string Metrics::timestamp() const
{
    auto now = std::chrono::system_clock::now();

    std::time_t t =
            std::chrono::system_clock::to_time_t(now);

    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");

    return oss.str();
}

void Metrics::record(int step, int interval, const PhaseField& Pfield, const TemperatureField& Tfield) {
    if (step % interval == 0) {

        double area = 0;
        double T_sum = 0;
        int T_cnt = 0;
        auto now = std::chrono::steady_clock::now();
        double wallTime =
                std::chrono::duration<double>(now - startTime_).count();

        for (unsigned int i = 0; i < Pfield.sizeX(); ++i) {
            for (unsigned int j = 0; j < Pfield.sizeY(); ++j) {
                float phi = Pfield.at(i, j);
                if (phi > 0) area++;
                if (std::abs(phi) < 0.9f) {
                    T_sum += Tfield.at(i, j);
                    T_cnt++;
                }
            }
        }

        csv_ << step << ","
             << wallTime << ","
             << area << ","
             << std::sqrt(area / M_PI) << ","
             << (T_cnt ? T_sum / T_cnt : 0.0)
             << "\n";

        csv_.flush();
    }
}
void Metrics::saveImage(int step, int interval, const FieldRenderer& FRenderer)
{
    if (step % interval != 0) return;

    std::string filename =
            (run_dir_ / "frames" /
             ("frame_" + std::to_string(step) + ".png")).string();

    if (!FRenderer.getImage().saveToFile(filename)) {
        std::cerr << "Failed to save image\n";
    }
}