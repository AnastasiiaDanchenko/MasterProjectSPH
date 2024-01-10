#include "..\headers\Grid.h"

std::vector<Particle> particles;
std::vector<std::vector<std::list<Particle*>>> grid;
std::vector<std::list<Particle*>> linearGrid;
std::vector<size_t> particleIndices;

int GRID_WIDTH;
int GRID_HEIGHT;

// Initialize uniformed grid of fluid particles
void InitFluid(const int l) {
    if (SIMULATION == "Initial") {
        for (int i = 0; i < PARTICLES_PER_DIMENSION * l; i++) {
            for (int j = 0; j < PARTICLES_PER_DIMENSION; j++) {
                Particle p;

                // + 4 to leave a boundary of 3 particles
                p.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
                p.ID = particles.size();
                particles.push_back(p);
            }
        }
    }
    else if (SIMULATION == "Dam break") {
        for (int i = 0; i < PARTICLES_PER_DIMENSION * 2; i++) {
            for (int j = 0; j < PARTICLES_PER_DIMENSION; j++) {
                Particle p;

                // + 4 to leave a boundary of 3 particles
                p.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
                p.ID = particles.size();
                particles.push_back(p);
            }
        }
    }
    else if (SIMULATION == "Complex boundaries") {
        for (int i = 0; i < PARTICLES_PER_DIMENSION; i++) {
            for (int j = 0; j < PARTICLES_PER_DIMENSION; j++) {
                Particle p, p1, p2;
                p.position = Eigen::Vector2f((i + 15) * SPACING, (j + 50) * SPACING);
                p1.position = Eigen::Vector2f((i + 45) * SPACING, (j + 45) * SPACING);
                p2.position = Eigen::Vector2f((i + 70) * SPACING, (j + 45) * SPACING);
                p.ID = particles.size();
                p1.ID = particles.size();
                p2.ID = particles.size();
                particles.push_back(p);
                particles.push_back(p1);
                particles.push_back(p2);
            }
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
                p.ID = particles.size();

                particles.push_back(p);
            }
        }
    }

    if (SIMULATION == "Complex boundaries") {
        // Place several boxes in the middle of the domain
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 3; j++) {
                Particle p, p1, p2, p3, p4;
                p.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
                p.isFluid = false;
                // Rotate the box by 45 degrees
                p.position = Eigen::Rotation2Df(120 / M_PI) * p.position;
                p.position += Eigen::Vector2f(SPACING * 75, SPACING * 20);
                p.ID = particles.size();
                particles.push_back(p);

                p1.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
                p1.isFluid = false;
                p1.position = Eigen::Rotation2Df(-60 / M_PI) * p1.position;
                p1.position += Eigen::Vector2f(SPACING * 15, SPACING * 40);
                p1.ID = particles.size();
                particles.push_back(p1);

                p2.position = Eigen::Vector2f((i + 45) * SPACING, (j + 16) * SPACING);
                p2.isFluid = false;
                p2.ID = particles.size();
                particles.push_back(p2);

                p3.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
                p3.isFluid = false;
                p3.position = Eigen::Rotation2Df(180 / M_PI) * p3.position;
                p3.position += Eigen::Vector2f(SPACING * 90, SPACING * 5);
                p3.ID = particles.size();
                particles.push_back(p3);

                p4.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
                p4.isFluid = false;
                p4.position = Eigen::Rotation2Df(-180 / M_PI) * p4.position;
                p4.position += Eigen::Vector2f(SPACING * 5, SPACING * 25);
                p4.ID = particles.size();
                particles.push_back(p4);
            }
        }
    }
}

void UniformGrid() {
    GRID_WIDTH = std::ceil(WINDOW_WIDTH / CELL_SIZE);
    GRID_HEIGHT = std::ceil(WINDOW_HEIGHT / CELL_SIZE);
    std::cout << "Using uniform grid with " << GRID_WIDTH << "x" << GRID_HEIGHT << " cells" << std::endl;

    grid.resize(GRID_WIDTH);
    for (int i = 0; i < GRID_WIDTH; i++) {
        grid[i].resize(GRID_HEIGHT);
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
        const int x = p.getCellNumber().x();
        const int y = p.getCellNumber().y();
        if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
            continue;
        }
        grid[x][y].push_back(&p);
    }
}

void LinearGrid() {
    //GRID_WIDTH = std::ceil(WINDOW_WIDTH / CELL_SIZE);
    //GRID_HEIGHT = std::ceil(WINDOW_HEIGHT / CELL_SIZE);
    
    GRID_WIDTH = std::pow(2, std::floor(std::log2(WINDOW_WIDTH / CELL_SIZE)));
    GRID_HEIGHT = std::pow(2, std::floor(std::log2(WINDOW_HEIGHT / CELL_SIZE)));

    std::cout << "Using uniform grid with " << GRID_WIDTH << "x" << GRID_HEIGHT << " cells" << std::endl;

    linearGrid.resize(GRID_WIDTH * GRID_HEIGHT);
    particleIndices.resize(particles.size());
    std::iota(particleIndices.begin(), particleIndices.end(), 0);
}

int LinearGridCellNumber(const int x, const int y) {
    int k = (int)(x / CELL_SIZE);
    int l = (int)(y / CELL_SIZE);
    return k + l * GRID_WIDTH;
}

uint64_t MortonCode(const int x, const int y) {
    uint64_t morton = 0;
    for (int i = 0; i < sizeof(uint64_t) * CHAR_BIT / 2; ++i) {
        morton |= ((x & (1ULL << i)) << i) | ((y & (1ULL << i)) << (i + 1));
    }
    return morton;
}

void Sorting() {
    /*std::sort(particleIndices.begin(), particleIndices.end(), [](const int a, const int b) {
		return LinearGridCellNumber(particles[a].position.x(), particles[a].position.y()) < 
               LinearGridCellNumber(particles[b].position.x(), particles[b].position.y());
		});*/

    std::sort(particleIndices.begin(), particleIndices.end(), [](const int a, const int b) {
		return MortonCode(particles[a].position.x(), particles[a].position.y()) < 
			   MortonCode(particles[b].position.x(), particles[b].position.y());
		});

    linearGrid.clear();
    linearGrid.resize(GRID_WIDTH * GRID_HEIGHT);

    for (auto& i : particleIndices) {
        Particle& p = particles[i];
        int cellNumber = LinearGridCellNumber(p.position.x(), p.position.y());

        if (cellNumber < 0 || cellNumber >= GRID_WIDTH * GRID_HEIGHT) { continue; }
        linearGrid[cellNumber].push_back(&p);
    }
}
