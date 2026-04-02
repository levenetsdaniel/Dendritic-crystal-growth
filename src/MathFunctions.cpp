#include <cmath>

double p(double phi) {
    return pow(phi, 3) * (10 - 15*phi - 6*pow(phi, 2));
}

double q(double phi) {
    return pow(phi, 2) * (1 - pow(phi, 2));
}

double Laplasian(double center, double up, double down, double right, double left, double dx, double dy) {
    return (up + down - 2*center) / pow(dx,2) + (left + right - 2 * center) / pow(dy, 2);
}


