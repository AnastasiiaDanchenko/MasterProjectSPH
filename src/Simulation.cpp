#include "..\headers\Simulation.h"

void Simulation() {
    if (NS_METHOD == "Quadratic search") { NeighborSearch(); }
    else if (NS_METHOD == "Verlet list") { NSVerletList(); } 
    else if (NS_METHOD == "Uniform grid") { NSUniformGrid(); }
    else if (NS_METHOD == "Spatial hashing") { NSHashTable(); }
    else {
		std::cout << "Invalid neighbor search method!" << std::endl;
		std::exit(1);
	}

    ComputeDensityPressure();
    ComputeAcceleration();
    UpdateParticles();
}

void Initialization() {
    InitBoundaries();
    InitFluid();
    if (NS_METHOD == "Uniform grid") {
        UniformGrid();
    }
    else if (NS_METHOD == "Spatial hashing") {
        HashTable();
    }
}
