#include "..\headers\ComputationsSPH.h"

// Loop through all particles and compute density
void ComputeDensity() {
    for (int i = 0; i < particles.size(); i++) {
        // Skip boundary particles
        if (particles[i].isFluid == false) {
            continue;
        }

        float density = 0.0f;
        for (int j = 0; j < particles[i].neighbors.size(); j++) {
            float dX = particles[i].x - particles[i].neighbors[j]->x;
            float dY = particles[i].y - particles[i].neighbors[j]->y;

            float distance = std::sqrt(dX * dX + dY * dY);

            density += particles[i].neighbors[j]->mass * CubicSplineKernel(distance, SPACING);
        }

        particles[i].density = density;
    }
}

// Loop through all particles and compute pressure
void ComputePressure() {
    for (int i = 0; i < particles.size(); i++) {
        // Skip boundary particles
        if (particles[i].isFluid == false) {
            continue;
        }
        particles[i].pressure = std::max(0.0f, STIFFNESS * (particles[i].density / REST_DENSITY - 1));
        if (particles[i].pressure != 0 && i != 136) {
            int a = 0;
        }
    }
}

// Loop through all particles and compute acceleration
void ComputeAcceleration() {
    for (int i = 0; i < particles.size(); i++) {
        // Skip boundary particles
        if (particles[i].isFluid == false) {
            continue;
        }

        float aPressureX = 0.0f;
        float aPressureY = 0.0f;
        float aViscosityX = 0.0f;
        float aViscosityY = 0.0f;

        float aGravity = -9.81f;

        for (int j = 0; j < particles[i].neighbors.size(); j++) {
            // Compute distance between particles
            float dX = particles[i].x - particles[i].neighbors[j]->x;
            float dY = particles[i].y - particles[i].neighbors[j]->y;

            float distance = std::sqrt(dX * dX + dY * dY);

            float dVX = particles[i].vx - particles[i].neighbors[j]->vx;
            float dVY = particles[i].vy - particles[i].neighbors[j]->vy;

            float dV = std::sqrt(dVX * dVX + dVY * dVY);

            // Compute acceleration
            aViscosityX += particles[i].neighbors[j]->mass / particles[i].neighbors[j]->density *
                dVX * dX / (dX * dX + 0.01 * SPACING * SPACING) * CubicSplineKernelGradient(distance, SPACING);

            aViscosityY += particles[i].neighbors[j]->mass / particles[i].neighbors[j]->density *
                dVY * dY / (dY * dY + 0.01 * SPACING * SPACING) * CubicSplineKernelGradient(distance, SPACING);

            if (particles[i].neighbors[j]->isFluid == false) {
                aPressureX += dX / distance * particles[i].neighbors[j]->mass * (2 * particles[i].pressure /
                    (particles[i].density * particles[i].density)) * CubicSplineKernelGradient(distance, SPACING);
                aPressureY += dY / distance * particles[i].neighbors[j]->mass * (2 * particles[i].pressure /
                    (particles[i].density * particles[i].density)) * CubicSplineKernelGradient(distance, SPACING);
            }
            else {
                if (distance == 0) {
                    continue;
                }
                else {
                    aPressureX += dX / distance * particles[i].neighbors[j]->mass * (particles[i].pressure /
                        (particles[i].density * particles[i].density) + particles[i].neighbors[j]->pressure /
                        (particles[i].neighbors[j]->density * particles[i].neighbors[j]->density)) *
                        CubicSplineKernelGradient(distance, SPACING);
                    aPressureY += dY / distance * particles[i].neighbors[j]->mass * (particles[i].pressure /
                        (particles[i].density * particles[i].density) + particles[i].neighbors[j]->pressure /
                        (particles[i].neighbors[j]->density * particles[i].neighbors[j]->density)) *
                        CubicSplineKernelGradient(distance, SPACING);
                }
            }
        }

        // Update acceleration
        particles[i].ax = -2 * VISCOSITY * aViscosityX + aPressureX;
        particles[i].ay = -2 * VISCOSITY * aViscosityY + aPressureY + aGravity;
    }
}

// Euler integration
void UpdateParticles() {
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i].isFluid == false) {
            continue;
        }
        particles[i].vx += particles[i].ax * TIME_STEP;
        particles[i].vy += particles[i].ay * TIME_STEP;
        particles[i].x += particles[i].vx * TIME_STEP;
        particles[i].y += particles[i].vy * TIME_STEP;
    }
}
