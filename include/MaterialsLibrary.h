#ifndef CRYSTAL_MATERIALSLIBRARY_H
#define CRYSTAL_MATERIALSLIBRARY_H

#include "MaterialsParametrs.h"
#include <string>
#include <map>
#include <stdexcept>

// ============================================================
// Библиотека материалов для симуляции кристаллизации
// ============================================================
//
// Содержит предустановки параметров для различных веществ:
// - SCN (сукцинонитрил)
// - Серебро
// - Оксид алюминия
// - Ледяной фронт
// - Кремний

class MaterialsLibrary {
public:
    enum class MaterialType {
        SCN,
        SILVER,
        ALUMINUM_OXIDE,
        ICE,
        SILICON
    };

    static Parameters getMaterial(MaterialType type);

    static Parameters getMaterialByName(const std::string& name);

    static void printAvailableMaterials();

private:
    static Parameters scn() {
        Parameters p;
        p.T_m    = 331.24;       // температура плавления, K
        p.L      = 46.24e7;      // скрытая теплота, эрг/см^3
        p.c_p    = 2.0e7;        // объёмная теплоёмкость, эрг/(см^3 K)
        p.D      = 1.27e-3;      // термодиффузия, см^2/с
        p.d0     = 2.8e-7;       // капиллярная длина, см
        p.eps4   = 0.0055;       // анизотропия
        p.beta0  = 0.0;          // кинетический коэф.

        p.W0_over_d0 = 100.0;
        p.dx_over_W0 = 0.4;
        p.dt_safety  = 0.008;
        p.Delta = 0.55;
        p.noise_amp = 0.0;
        return p;
    }

    static Parameters silver() {
        Parameters p;
        p.T_m    = 1234.93;      // температура плавления Ag, K
        p.L      = 2.55e9;       // скрытая теплота, эрг/см^3
        p.c_p    = 2.49e7;       // объёмная теплоёмкость, эрг/(см^3 K)
        p.D      = 0.15e-4;      // термодиффузия в расплаве, см^2/с
        p.d0     = 1.5e-8;       // капиллярная длина, см
        p.eps4   = 0.02;
        p.beta0  = 0.1;

        p.W0_over_d0 = 200.0;
        p.dx_over_W0 = 0.3;
        p.dt_safety  = 0.005;
        p.Delta = 0.3;
        p.noise_amp = 0.0;
        return p;
    }

    static Parameters aluminumOxide() {
        Parameters p;
        p.T_m    = 2326.0;       // температура плавления Al2O3, K
        p.L      = 1.13e10;      // скрытая теплота, эрг/см^3
        p.c_p    = 3.2e7;        // объёмная теплоёмкость, эрг/(см^3 K)
        p.D      = 0.05e-4;      // очень низкая теплопроводность
        p.d0     = 3.0e-8;
        p.eps4   = 0.003;        // слабая анизотропия
        p.beta0  = 0.05;

        p.W0_over_d0 = 150.0;
        p.dx_over_W0 = 0.35;
        p.dt_safety  = 0.006;
        p.Delta = 0.4;
        p.noise_amp = 0.0;
        return p;
    }

    static Parameters ice() {
        Parameters p;
        p.T_m    = 273.15;       // температура плавления, K
        p.L      = 3.34e9;       // скрытая теплота замерзания, эрг/см^3
        p.c_p    = 4.218e7;      // объёмная теплоёмкость воды, эрг/(см^3 K)
        p.D      = 1.38e-3;      // теплопроводность воды, см^2/с
        p.d0     = 1.0e-7;       // капиллярная длина
        p.eps4   = 0.01;         // умеренная анизотропия
        p.beta0  = 0.0;

        p.W0_over_d0 = 80.0;
        p.dx_over_W0 = 0.5;
        p.dt_safety  = 0.01;
        p.Delta = 0.8;
        p.noise_amp = 0.001;
        return p;
    }

    static Parameters silicon() {
        Parameters p;
        p.T_m    = 1687.0;       // температура плавления Si, K
        p.L      = 1.81e9;       // скрытая теплота, эрг/см^3
        p.c_p    = 1.66e7;       // объёмная теплоёмкость, эрг/(см^3 K)
        p.D      = 0.2e-3;       // теплопроводность расплава, см^2/с
        p.d0     = 2.0e-8;       // капиллярная длина
        p.eps4   = 0.015;        // анизотропия
        p.beta0  = 0.05;

        p.W0_over_d0 = 120.0;
        p.dx_over_W0 = 0.4;
        p.dt_safety  = 0.007;
        p.Delta = 0.5;
        p.noise_amp = 0.0;
        return p;
    }
};

#endif //CRYSTAL_MATERIALSLIBRARY_H