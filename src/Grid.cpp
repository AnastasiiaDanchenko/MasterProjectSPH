#include "..\headers\Grid.h"

std::vector<Particle> particles;

// Initialize uniformed grid of fluid particles
void InitFluid() {
    for (int i = 0; i < PARTICLES_PER_DIMENSION; i++) {
        for (int j = 0; j < PARTICLES_PER_DIMENSION; j++) {
            Particle p;

            p.position = Eigen::Vector2f((i + 4) * SPACING, (j + 4) * SPACING);
            // + 4 to leave a boundary of 3 particles

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
