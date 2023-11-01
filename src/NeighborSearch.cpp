#include "..\headers\NeighborSearch.h"

// Standard Neighbor search
void NeighborSearch() {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].neighbors.clear();

        for (int j = 0; j < particles.size(); j++) {
            float dx = particles[j].x - particles[i].x;
            float dy = particles[j].y - particles[i].y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < SUPPORT) {
                particles[i].neighbors.push_back(&particles[j]);
            }
        }
    }
}
