#include "..\headers\Simulation.h"

void Simulation() {
    //NeighborSearch();
    //NSUniformGrid();
    NSVerletList();

    ComputeDensityPressure();
    ComputeAcceleration();
    UpdateParticles();
}