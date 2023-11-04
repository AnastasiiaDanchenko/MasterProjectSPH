#include "..\headers\NeighborSearch.h"

// Standard Neighbor search
void NeighborSearch() {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].neighbors.clear();

        for (int j = 0; j < particles.size(); j++) {
            float dx = particles[j].position.x() - particles[i].position.x();
            float dy = particles[j].position.y() - particles[i].position.y();
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < SUPPORT) {
                particles[i].neighbors.push_back(&particles[j]);
            }
        }
    }
}

void NSUniformGrid() {
    GridUpdate();

    for (int i = 0; i < particles.size(); i++) {
		particles[i].neighbors.clear();
        const int x = particles[i].getCellNumber().x();
        const int y = particles[i].getCellNumber().y();

        for (int j = x - 1; j <= x + 1; j++) {
            for (int k = y - 1; k <= y + 1; k++) {
                if (j < 0 || j >= GRID_WIDTH || k < 0 || k >= GRID_HEIGHT) {
					continue;
				}
                for (auto& p : grid[j][k]) {
					float dx = p->position.x() - particles[i].position.x();
					float dy = p->position.y() - particles[i].position.y();
					float distance = std::sqrt(dx * dx + dy * dy);
                    if (distance < SUPPORT) {
						particles[i].neighbors.push_back(p);
					}
				}
			}
		}
	}
}

void UpdateVerletList() {
    for (int i = 0; i < particles.size(); i++) {
		particles[i].candidates.clear();
        for (int j = 0; j < particles.size(); j++) {
			float dx = particles[j].position.x() - particles[i].position.x();
			float dy = particles[j].position.y() - particles[i].position.y();
			float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < 2 * SUPPORT) {
				particles[i].candidates.push_back(&particles[j]);
			}
		}
	}
}

void NSVerletList() {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].neighbors.clear();
        for (int j = 0; j < particles[i].candidates.size(); j++) {
			const Eigen::Vector2f r = particles[i].position - particles[i].candidates[j]->position;
            if (r.squaredNorm() < pow(SUPPORT, 2)) {
                particles[i].neighbors.push_back(particles[i].candidates[j]);
            }
        }
    }
}

void NSHashTable() {

}
