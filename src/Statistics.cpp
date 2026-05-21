#include "Statistics.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <iomanip>

static constexpr double PI = 3.14159265358979323846;

Statistics::Statistics(const PhaseField& field, uint64_t cx, uint64_t cy)
        : field_(field), cx_(cx), cy_(cy) {}

// бинарный поиск по лучу из (cx, cy).

double Statistics::radiusInDirection(double angle_deg) const {
    const double angle  = angle_deg * PI / 180.0;
    const double cos_a  = std::cos(angle);
    const double sin_a  = std::sin(angle);

    const uint64_t nx   = field_.sizeX();
    const uint64_t ny   = field_.sizeY();
    const double max_r  = std::min(cx_, cy_) - 2.0;

    double lo = 0.0, hi = max_r;

    // Грубый проход с шагом 1 ячейка — найти диапазон перехода
    double r_cross = max_r;
    for (double r = 0.0; r <= max_r; r += 1.0) {
        long long xi = static_cast<long long>(cx_) + std::llround(r * cos_a);
        long long yj = static_cast<long long>(cy_) + std::llround(r * sin_a);
        if (xi < 0 || yj < 0 ||
            static_cast<uint64_t>(xi) >= nx ||
            static_cast<uint64_t>(yj) >= ny) { r_cross = r; break; }
        if (field_.at(static_cast<uint64_t>(xi),
                      static_cast<uint64_t>(yj)) < 0.0f) {
            r_cross = r;
            break;
        }
    }

    // Бинарный поиск в окрестности перехода
    lo = std::max(0.0, r_cross - 3.0);
    hi = std::min(max_r, r_cross + 1.0);

    for (int iter = 0; iter < 20; ++iter) {
        double mid = 0.5 * (lo + hi);
        long long xi = static_cast<long long>(cx_) + std::llround(mid * cos_a);
        long long yj = static_cast<long long>(cy_) + std::llround(mid * sin_a);
        if (xi < 0 || yj < 0 ||
            static_cast<uint64_t>(xi) >= nx ||
            static_cast<uint64_t>(yj) >= ny) { hi = mid; continue; }
        float phi = field_.at(static_cast<uint64_t>(xi),
                              static_cast<uint64_t>(yj));
        if (phi >= 0.0f) lo = mid; else hi = mid;
    }
    return 0.5 * (lo + hi);
}

// число локальных максимумов с prominence.
int Statistics::countPeaks(const std::vector<double>& R, double prominence) {
    const int n = static_cast<int>(R.size());
    int count = 0;
    for (int i = 0; i < n; ++i) {
        double prev = R[(i - 1 + n) % n];
        double curr = R[i];
        double next = R[(i + 1) % n];
        if (curr > prev && curr > next && curr - std::min(prev, next) > prominence)
            ++count;
    }
    return count;
}

//  ищет кратность 1..8 с наибольшей амплитудой
int Statistics::dominantSymmetry(const std::vector<double>& R) {
    const int n = static_cast<int>(R.size());
    int    best_k = 1;
    double best_A = -1.0;

    for (int k = 1; k <= 8; ++k) {
        double sum_c = 0.0, sum_s = 0.0;
        for (int i = 0; i < n; ++i) {
            double theta = 2.0 * PI * i / n;
            sum_c += R[i] * std::cos(k * theta);
            sum_s += R[i] * std::sin(k * theta);
        }
        double A = std::sqrt(sum_c*sum_c + sum_s*sum_s) / n;
        if (A > best_A) { best_A = A; best_k = k; }
    }
    return best_k;
}

// заполняет result_.
void Statistics::compute() {
    // Площадь
    result_.area = 0.0;
    for (uint64_t i = 0; i < field_.sizeX(); ++i)
        for (uint64_t j = 0; j < field_.sizeY(); ++j)
            if (field_.at(i, j) > 0.0f) result_.area += 1.0;

    // R(θ)
    result_.R.resize(N_ANGLES);
    for (int a = 0; a < N_ANGLES; ++a)
        result_.R[a] = radiusInDirection(static_cast<double>(a));

    result_.radius_mean = std::accumulate(result_.R.begin(), result_.R.end(), 0.0)
                          / N_ANGLES;
    result_.radius_max  = *std::max_element(result_.R.begin(), result_.R.end());
    result_.radius_min  = *std::min_element(result_.R.begin(), result_.R.end());
    result_.anisotropy  = (result_.radius_min > 0.0)
                          ? result_.radius_max / result_.radius_min : 0.0;

    // Ветви: порог prominence = 5% от среднего радиуса
    double prom         = 0.05 * result_.radius_mean;
    result_.branch_count   = countPeaks(result_.R, prom);
    result_.symmetry_order = dominantSymmetry(result_.R);
}


void Statistics::printReport() const {
    const auto& r = result_;
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              <<   "║         DENDRITIC STATISTICS             ║\n"
              <<   "╠══════════════════════════════════════════╣\n"
              << std::fixed << std::setprecision(2)
              << "║  Solid area     : " << std::setw(10) << r.area
              << " cells          ║\n"
              << "║  Radius mean    : " << std::setw(10) << r.radius_mean
              << " cells          ║\n"
              << "║  Radius max     : " << std::setw(10) << r.radius_max
              << " cells          ║\n"
              << "║  Radius min     : " << std::setw(10) << r.radius_min
              << " cells          ║\n"
              << "║  Anisotropy     : " << std::setw(10) << r.anisotropy
              << " (max/min)      ║\n"
              << "║  Branches       : " << std::setw(10) << r.branch_count
              << "                ║\n"
              << "║  Symmetry order : " << std::setw(10) << r.symmetry_order
              << "-fold           ║\n"
              <<   "╚══════════════════════════════════════════╝\n\n";
}


void Statistics::saveToCsv(const std::string& path) const {
    std::filesystem::create_directories(
            std::filesystem::path(path).parent_path());

    std::ofstream f(path);
    if (!f) { std::cerr << "[Statistics] Не удалось открыть: " << path << "\n"; return; }

    // Сводка
    f << "# area,"       << result_.area           << "\n"
      << "# radius_mean," << result_.radius_mean   << "\n"
      << "# radius_max,"  << result_.radius_max    << "\n"
      << "# radius_min,"  << result_.radius_min    << "\n"
      << "# anisotropy,"  << result_.anisotropy    << "\n"
      << "# branches,"    << result_.branch_count  << "\n"
      << "# symmetry,"    << result_.symmetry_order << "\n"
      << "angle_deg,radius_cells\n";

    for (int a = 0; a < N_ANGLES; ++a)
        f << a << "," << result_.R[a] << "\n";

    std::cout << "[Statistics] Сохранено: " << path << "\n";
}