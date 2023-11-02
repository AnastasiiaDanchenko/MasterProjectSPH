#include "..\headers\Kernel.h"

// Compute cubic spline kernel function
float CubicSplineKernel(Eigen::Vector2f r) {
    float alpha = 10 / (7 * M_PI * pow(SPACING, 2));
    float q = r.norm() / SPACING;

    if (q >= 0 && q < 1) {
		return alpha * (1 - 1.5 * pow(q, 2) + 0.75 * pow(q, 3));
	} else if (q >= 1 && q < 2) {
		return alpha * 0.25 * pow(2 - q, 3);
	}
    return 0.0f;
}

// Compute cubic spline kernel gradient
Eigen::Vector2f CubicSplineKernelGradient(Eigen::Vector2f r) {
    float alpha = 10 / (7 * M_PI * pow(SPACING, 2));
    float q = r.norm() / SPACING;
    float derivative = 0.0f;
    
    if (q >= 0 && q < 1) {
        derivative = alpha / SPACING * (-3 * q + 2.25 * pow(q, 2));
    } else if (q >= 1 && q < 2) {
        derivative = alpha / SPACING * (-0.75 * pow(2 - q, 2));
	}
    return derivative * r.normalized();
}

// Test the kernel function and its gradient for uniformed grid of particles
//void KernelTest(const float radius, const int numParticles) {
//    for (int i = 0; i < particles.size(); i++) {
//        float kernelSum = 0.0f;
//        float kernelGradientSum = 0.0f;
//
//        int sgn = 1;
//        const float volumeRev = round(1 / (radius * radius) * 100) / 100;
//        const float radiusRevNeg = round(-1 / radius * 100) / 100;
//
//        if (particles[i].neighbors.size() < 13) {
//            continue;
//        }
//
//        for (int j = 0; j < particles[i].neighbors.size(); j++) {
//            float dX = particles[i].position.x() - particles[i].neighbors[j]->position.x();
//            float dY = particles[i].position.y() - particles[i].neighbors[j]->position.y();
//
//            float distance = std::sqrt(dX * dX + dY * dY);
//
//            dX = round(dX * 100000) / 100000;
//            if (dX > 0 || dX == 0 && dY > 0) {
//                sgn = -1;
//            }
//            else {
//                sgn = 1;
//            }
//
//            kernelSum += CubicSplineKernel(distance, radius);
//            float temp = CubicSplineKernelGradient(distance, radius);
//            kernelGradientSum += sgn * temp;
//        }
//
//        kernelSum = round(kernelSum * 100) / 100;
//        kernelGradientSum = round(kernelGradientSum * 100) / 100;
//
//        if (kernelSum - volumeRev >= kernelSum / 50) { // 2% error
//            std::cout << "Kernel Sum test failed" << std::endl;
//        }
//        if (kernelGradientSum != 0) {
//            std::cout << "Kernel Gradient Sum test failed" << std::endl;
//        }
//    }
//}