#pragma once


#include <limits>


const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

inline float degreesToRadians(const float& degrees) {
    return degrees * pi / 180;
}

