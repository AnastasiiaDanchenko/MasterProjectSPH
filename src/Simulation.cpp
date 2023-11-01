#include "..\headers\Simulation.h"

void Simulation() {
    NeighborSearch();
    ComputeDensity();
    ComputePressure();
    ComputeAcceleration();
    UpdateParticles();
}