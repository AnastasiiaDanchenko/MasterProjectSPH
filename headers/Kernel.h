#pragma once
#include <algorithm>

#include "Grid.h"

float CubicSplineKernel(float distance, float radius);
float CubicSplineKernelGradient(float distance, float radius);
void KernelTest(const float radius, const int numParticles);
