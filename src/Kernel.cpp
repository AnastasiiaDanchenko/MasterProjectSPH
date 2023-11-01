#include "..\headers\Kernel.h"

// Compute cubic spline kernel function
float CubicSplineKernel(float distance, const float radius) {
    float q = distance / radius;
    float result = 0.0f;
    float alpha = 5 / (14 * M_PI * radius * radius);

    float t1 = std::max(0.0f, 1.0f - q);
    float t2 = std::max(0.0f, 2.0f - q);

    result = alpha * (t2 * t2 * t2 - 4.0f * t1 * t1 * t1);

    return result;
}

// Compute cubic spline kernel gradient
float CubicSplineKernelGradient(float distance, const float radius) {
    float q = distance / radius;
    float result = 0.0f;
    float alpha = 5 / (14 * M_PI * radius * radius);

    float t1 = std::max(0.0f, 1.0f - q);
    float t2 = std::max(0.0f, 2.0f - q);

    result = alpha * (3.0f * t2 * t2 - 12.0f * t1 * t1);

    return result;
}

// Test the kernel function and its gradient for uniformed grid of particles
void KernelTest(const float radius, const int numParticles) {
    for (int i = 0; i < particles.size(); i++) {
        float kernelSum = 0.0f;
        float kernelGradientSum = 0.0f;

        int sgn = 1;
        const float volumeRev = round(1 / (radius * radius) * 100) / 100;
        const float radiusRevNeg = round(-1 / radius * 100) / 100;

        if (particles[i].neighbors.size() < 13) {
            continue;
        }

        for (int j = 0; j < particles[i].neighbors.size(); j++) {
            float dX = particles[i].x - particles[i].neighbors[j]->x;
            float dY = particles[i].y - particles[i].neighbors[j]->y;

            float distance = std::sqrt(dX * dX + dY * dY);

            dX = round(dX * 100000) / 100000;
            if (dX > 0 || dX == 0 && dY > 0) {
                sgn = -1;
            }
            else {
                sgn = 1;
            }

            kernelSum += CubicSplineKernel(distance, radius);
            float temp = CubicSplineKernelGradient(distance, radius);
            kernelGradientSum += sgn * temp;
        }

        kernelSum = round(kernelSum * 100) / 100;
        kernelGradientSum = round(kernelGradientSum * 100) / 100;

        if (kernelSum - volumeRev >= kernelSum / 50) { // 2% error
            std::cout << "Kernel Sum test failed" << std::endl;
        }
        if (kernelGradientSum != 0) {
            std::cout << "Kernel Gradient Sum test failed" << std::endl;
        }
    }
}