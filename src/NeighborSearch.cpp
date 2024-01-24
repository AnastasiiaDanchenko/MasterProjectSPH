#include "..\headers\NeighborSearch.h"

// Standard Neighbor search
void QuadraticSearch() {
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
                /*for (auto& p : grid[j][k]) {
					float dx = p->position.x() - particles[i].position.x();
					float dy = p->position.y() - particles[i].position.y();
					float distance = std::sqrt(dx * dx + dy * dy);
                    if (distance < SUPPORT) {
						particles[i].neighbors.push_back(p);
					}
				}*/
                for (auto& p : grid[j + k * GRID_WIDTH].cellParticles) {
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
    HashTableUpdate();

    for (int i = 0; i < particles.size(); i++) {
		particles[i].neighbors.clear();
        
        std::unordered_map<int, int> miniTable;
        for (int m = -1; m <= 1; m++) {
            for (int n = -1; n <= 1; n++) {
				const int c = HashFunction(particles[i].position + Eigen::Vector2f(m * CELL_SIZE, n * CELL_SIZE));
                if (miniTable[c] > 0) miniTable[c]++;
                else miniTable.emplace(c, 1);
			}
        }

        for (auto element : miniTable) {
            for (int j = 0; j < hashTable[element.first].size(); j++) {
                const Eigen::Vector2f r = particles[i].position - hashTable[element.first][j]->position;
                if (r.squaredNorm() < pow(SUPPORT, 2)) {
                    particles[i].neighbors.push_back(hashTable[element.first][j]);
                }
            }
        }
	}
}

void NSSorting() {
    Sorting();
    for (int i : particleIndices) {
        particles[i].neighbors.clear();

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int neighborCell = LinearGridCellNumber(particles[i].position.x() + dx * CELL_SIZE, particles[i].position.y() + dy * CELL_SIZE);

                if (neighborCell < 0 || neighborCell >= GRID_WIDTH * GRID_HEIGHT) { continue; }

                for (auto& p : linearGrid[neighborCell]) {
					const Eigen::Vector2f r = particles[i].position - p->position;
                    if (r.squaredNorm() < pow(SUPPORT, 2)) {
						particles[i].neighbors.push_back(p);
					}
				}
            }
        }
    }
}

void NSOctree() {
    GridUpdateIncremental();
    /*OctreeInit(Eigen::Vector2f(0, 0), Eigen::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT), 10);
    SplitNode();*/
    OctreeSearch();
}
