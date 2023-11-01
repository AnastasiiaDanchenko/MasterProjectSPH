#pragma once
#include "Particle.h"

// Particle container
extern std::vector<Particle> particles;

void InitFluid();
void InitBoundaries();

void RotateGrid(float angle);
void TranslateGrid(float x, float y);
