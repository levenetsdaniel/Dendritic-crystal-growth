#ifndef CRYSTAL_METRICS_H
#define CRYSTAL_METRICS_H

#include <iostream>
#include <fstream>
#include "PhaseField.h"
#include "FieldRender.h"
#include <cmath>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>


class Metrics {
    std::ofstream csv_;
    std::string run_id_;
    std::chrono::steady_clock::time_point startTime_;
    std::string timestamp() const;
public:
    Metrics(const Parameters&);
    void record(int step, int frame, const PhaseField&, const TemperatureField&);
    void saveImage(int step, int interval, const FieldRenderer&);
};

#endif //CRYSTAL_METRICS_H
