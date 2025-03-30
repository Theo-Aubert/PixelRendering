#pragma once

#include <cstdlib>

#define RESSOURCE_PATH std::string(".../../Ressources/")

const double PI = 3.141592653589793238463;
const float  PI_F = 3.14159265358979f;

const double infinity = std::numeric_limits<double>::infinity();

inline double Deg2Rad(double degrees) { return degrees * PI / 180; }

inline double RandomDouble() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline double RandomDouble(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * RandomDouble();
}

inline double RandomInt(int min, int max) {
    // Returns a random integer in [min,max).
    return int(RandomDouble(min,max +1));
}