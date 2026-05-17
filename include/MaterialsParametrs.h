#ifndef CRYSTAL_MATERIALSPARAMETRS_H
#define CRYSTAL_MATERIALSPARAMETRS_H

struct Parameters {
    // Сетка
    double dx = 0.1, dy = 0.1;
    double dt = 0.00005;

    // Фазовое поле
    double M     = 1.0;
    double W     = 0.3;

    // Тепло
    double D     = 1.0;    // термодиффузия
    double Cp    = 1.0;
    double L     = 0.5;    // скрытая теплота

    // Анизотропия
    double eps   = 0.05;   // сила анизотропии
    double delta = 4.0;    // режим симметрии

    double alpha = 0.9;
    double gamma = 10.0;

    // Температура
    double Tm     = 1.0;   // температура плавления
    double T_init = 0.55;  // начальная температура расплава


    double noise  = 0.01;

    double dla_sensitivity = 3.0;
};

#endif