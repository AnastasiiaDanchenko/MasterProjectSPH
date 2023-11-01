#include "..\headers\Grid.h"

std::vector<Particle> particles;

// Initialize uniformed grid of fluid particles
void InitFluid() {
    for (int i = 0; i < PARTICLES_PER_DIMENSION; i++) {
        for (int j = 0; j < PARTICLES_PER_DIMENSION; j++) {
            Particle p;

            p.x = (i + 4) * SPACING;
            p.y = (j + 4) * SPACING;

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

                p.x = (i + 1) * SPACING;
                p.y = (j + 1) * SPACING;
                p.isFluid = false;

                particles.push_back(p);
            }
        }
    }
}

// Grid rotation for testing of the neighbor search
void RotateGrid(float angle) {
    // Convert angle from degrees to radians
    float radians = angle * M_PI / 180.0f;

    for (int i = 0; i < particles.size(); i++) {
        float x = particles[i].x;
        float y = particles[i].y;

        float rotatedX = x * cos(radians) - y * sin(radians);
        float rotatedY = x * sin(radians) + y * cos(radians);

        particles[i].x = rotatedX;
        particles[i].y = rotatedY;
    }
}

// Grid translation for testing of the neighbor search
void TranslateGrid(float x, float y) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].x += x;
        particles[i].y += y;
    }
}
