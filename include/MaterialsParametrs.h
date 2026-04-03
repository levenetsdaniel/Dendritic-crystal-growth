#ifndef CRYSTAL_MATERIALSPARAMETRS_H
#define CRYSTAL_MATERIALSPARAMETRS_H

struct Parameters {
    double dx = 0.1;
    double dy = 0.1;

    double dt = 5e-5;

    // Phase field mobility (ОЧЕНЬ важно уменьшить)
    double M = 0.15;

    double a = 1.0;

    // интерфейсная энергия — уменьшена
    double W = 0.3;

    // теплопроводность
    double D = 0.2;

    // связь фазы и температуры — ослаблена
    double df = 0.2;

    double kappa = 1.0;
    double Cp = 1.0;

    // скрытая теплота — уменьшена
    double L = 0.08;

    // анизотропия — сильно уменьшена
    double eps = 0.03;
    double delta = 4;

    double Tm = 0.0;

    // шум почти убран (критично!)
    double noise = 0.0002;
};

#endif //CRYSTAL_MATERIALSPARAMETRS_H