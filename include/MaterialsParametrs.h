#ifndef CRYSTAL_MATERIALSPARAMETRS_H
#define CRYSTAL_MATERIALSPARAMETRS_H

struct Parameters {
    double T_m = 331.24; // температура плавления, K
    double L = 46.24e7; // скрытая теплота на объём, эрг/см^3
    double c_p = 2.0e7; // объёмная теплоёмкость, эрг/(см^3 K)
    double D = 1.27e-3; // термодиффузия в расплаве, см^2/с
    double d0 = 2.8e-7; // капиллярная длина, см
    double eps4 = 0.0055; // анизотропия поверхностного натяжения
    double beta0 = 0.0; // кинетический коэф. около 0 для SCN

    double dT_unit() const { return L / c_p; } // около 23.1 K


    double W0_over_d0 = 100.0; // ширина интерфейса в единицах d0
    double dx_over_W0 = 0.4; // разрешение интерфейса (узлов на W0)
    double dt_safety = 0.008; // запас по устойчивости диффузии

    double Delta = 0.55; // безразмерное переохлаждение, u_inf = -Delta
    // (соответствует T_init = T_m - Delta * dT_unit)

    double noise_amp = 0.0; // тепловой шум на интерфейсе


    // Константы асимптотики тонкого интерфейса (Karma-Rappel 1998)
    static constexpr double a1 = 0.8839;
    static constexpr double a2 = 0.6267;

    // Физическая ширина интерфейса, см
    double W0() const { return W0_over_d0 * d0; }

    // Безразмерная константа связи
    double lambda() const { return a1 * W0_over_d0; }

    // Характерное время релаксации фазового поля, с
    double tau0() const { return a2 * lambda() * W0() * W0() / D; }

    // Безразмерная диффузия
    double D_tilde() const { return a2 * lambda(); }

    // Шаги сетки и времени
    double dx() const { return dx_over_W0; }
    double dy() const { return dx_over_W0; }

    double dt() const {
        double h = dx();
        return dt_safety * h * h / D_tilde();
    }

    // Начальная безразмерная температура
    double u_init() const { return -Delta; }
};

#endif
