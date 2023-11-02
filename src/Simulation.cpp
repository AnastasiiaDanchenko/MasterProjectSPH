#include "..\headers\Simulation.h"

void Simulation() {
    //NeighborSearch();
    NSUniformGrid();

    ComputeDensityPressure();
    ComputeAcceleration();
    UpdateParticles();
}