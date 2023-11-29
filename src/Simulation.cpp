#include "..\headers\Simulation.h"

void NeighborSearch() {
    if (NS_METHOD == "Quadratic search") { QuadraticSearch(); }
    else if (NS_METHOD == "Verlet list") { NSVerletList(); }
    else if (NS_METHOD == "Uniform grid") { NSUniformGrid(); }
    else if (NS_METHOD == "Spatial hashing") { NSHashTable(); }
    else if (NS_METHOD == "Index Sorting") { NSSorting(); }
    else {
        std::cout << "Invalid neighbor search method!" << std::endl;
        std::exit(1);
    }
}

void Simulation() {
    NeighborSearch();
    ComputeDensityPressure();
    ComputeAcceleration();
    UpdateParticles();
}

void Initialization(const int l) {
    std::cout << "Using " << NS_METHOD << " as a neighbor search method" << std::endl;
    InitBoundaries();
    InitFluid(l);
    if (NS_METHOD == "Uniform grid") {
        UniformGrid();
    }
    else if (NS_METHOD == "Spatial hashing") {
        HashTable();
    }
    else if (NS_METHOD == "Index Sorting") {
        LinearGrid();
	}
}
