#include "..\headers\Simulation.h"

void Simulation() {
    NeighborSearch();
    ComputeDensityPressure();
    ComputeAcceleration();
    UpdateParticles();
}