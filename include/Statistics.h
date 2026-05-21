#ifndef CRYSTAL_STATISTICS_H
#include <filesystem>
#define CRYSTAL_STATISTICS_H

#include "PhaseField.h"
#include <vector>
#include <string>
#include <cstdint>

class Statistics {
public:
    static constexpr int N_ANGLES = 360;  // угловое разрешение (градусы)

    struct Result {
        double area;           // площадь в ячейках
        double radius_mean;    // средний радиус (ячейки)
        double radius_max;     // максимальный радиус
        double radius_min;     // минимальный радиус
        double anisotropy;     // R_max / R_min
        int    branch_count;   // число ветвей (локальных максимумов R(θ))
        int    symmetry_order; // доминирующая кратность (4 для дендрита)
        std::vector<double> R; // R(θ) — радиус по углу, N_ANGLES значений
    };

    Statistics(const PhaseField& field, uint64_t cx, uint64_t cy);

    void compute();

    void printReport() const;

    void saveToCsv(const std::string& path) const;

    const Result& result() const { return result_; }

private:
    const PhaseField& field_;
    uint64_t          cx_, cy_;
    Result            result_;


    double radiusInDirection(double angle_deg) const;

    static int countPeaks(const std::vector<double>& R, double prominence);

    static int dominantSymmetry(const std::vector<double>& R);
};

#endif // CRYSTAL_STATISTICS_H