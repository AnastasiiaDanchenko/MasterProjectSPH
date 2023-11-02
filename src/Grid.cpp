#include "..\headers\Grid.h"

std::vector<Particle> particles;
std::vector<std::vector<std::list<Particle*>>> grid;
int GRID_WIDTH;
int GRID_HEIGHT;

// Initialize uniformed grid of fluid particles
void InitFluid() {
    for (int i = 0; i < PARTICLES_PER_DIMENSION; i++) {
        for (int j = 0; j < PARTICLES_PER_DIMENSION; j++) {
            Particle p;

            // + 4 to leave a boundary of 3 particles
            p.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
            particles.push_back(p);
        }
    }
}

// Initialize boundaries
void InitBoundaries() {
    int width = WINDOW_WIDTH / (2 * SPACING) - 1;
    int hight = WINDOW_HEIGHT / (2 * SPACING) - 1;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < hight; j++) {
            if (i < 3 || i > width - 4 || j < 3 || j > hight - 4) {
                Particle p;

                p.position = Eigen::Vector2f((i + 1) * SPACING, (j + 1) * SPACING);
                p.isFluid = false;

                particles.push_back(p);
            }
        }
    }
}

void UniformGrid() {
    GRID_WIDTH = std::ceil(WINDOW_WIDTH / (4 * SPACING));
    GRID_HEIGHT = std::ceil(WINDOW_HEIGHT / (4 * SPACING));
    std::cout << GRID_WIDTH << " " << GRID_HEIGHT << std::endl;
    grid.resize(GRID_WIDTH);
    for (int i = 0; i < GRID_WIDTH; i++) {
		grid[i].resize(GRID_HEIGHT);
	}

    for (auto& p : particles) {
        int x = std::floor(p.position.x() / (2 * SPACING));
		int y = std::floor(p.position.y() / (2 * SPACING));
		grid[x][y].push_back(&p);
    }
}

void GridUpdate() {
    // Clear grid
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
			grid[i][j].clear();
		}
    }

    for (auto& p : particles) {
		int x = std::floor(p.position.x() / (2 * SPACING));
		int y = std::floor(p.position.y() / (2 * SPACING));
		grid[x][y].push_back(&p);
	}
}
