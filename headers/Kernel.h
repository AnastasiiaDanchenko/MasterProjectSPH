#pragma once
#include <algorithm>

#include "Grid.h"

float CubicSplineKernel(Eigen::Vector2f r);
Eigen::Vector2f CubicSplineKernelGradient(Eigen::Vector2f r);
//void KernelTest(const float radius, const int numParticles);
