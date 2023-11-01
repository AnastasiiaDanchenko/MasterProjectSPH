#include "..\headers\Particle.h"

Particle::Particle() :
    x(0.0f),
    y(0.0f),
    vx(0.0f),
    vy(0.0f),
    ax(0.0f),
    ay(0.0f),
    mass(SPACING * SPACING / REST_DENSITY),
    density(REST_DENSITY),
    pressure(0.0f),
    isFluid(true) {}
