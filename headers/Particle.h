#pragma once
#include <vector>

#include "Parameters.h"

class Particle {
public:
    float x;
    float y;
    float vx;
    float vy;
    float ax;
    float ay;

    float mass;
    float density;
    float pressure;

    bool isFluid;

    std::vector<Particle*> neighbors;

    Particle();
};
