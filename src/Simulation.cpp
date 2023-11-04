#include "..\headers\Simulation.h"

void Simulation() {
    if (NS_METHOD == "Quadratic search") { NeighborSearch(); }
    else if (NS_METHOD == "Verlet list") { NSVerletList(); } 
    else if (NS_METHOD == "Uniform grid") { NSUniformGrid(); }
    else {
		std::cout << "Invalid neighbor search method!" << std::endl;
		std::exit(1);
	}

    ComputeDensityPressure();
    ComputeAcceleration();
    UpdateParticles();
}