#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "headers/CompileShaders.h"
#include "headers/Parameters.h"
//#include "headers/SimulationFalling.h"

#include <iostream>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Close the window when pressing ESC
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
}

class Particle {
public:
    float x;
    float y;
    float vx;
    float vy;
    float ax;
    float ay;

    float mass;
    float density;
    float pressure;

    bool isFluid;

    std::vector<Particle*> neighbors;

    Particle();
};

Particle::Particle() :
    x(0.0f),
    y(0.0f),
    vx(0.0f),
    vy(0.0f),
    ax(0.0f),
    ay(0.0f),
    mass(0.0f),
    density(0.0f),
    pressure(0.0f),
    isFluid(true) {}


float rotationAngle = 45.0f;  // Rotation angle in degrees
float rotationAngleRad = rotationAngle * M_PI / 180.0f;  // Convert to radians

// Particle container
std::vector<Particle> particles;

// Initialize uniformed grid of fluid particles
void InitParticles() {
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

// Standard Neighbor search
void NeighborSearch(const float radius) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].neighbors.clear();

        for (int j = 0; j < particles.size(); j++) {
            float dx = particles[j].x - particles[i].x;
            float dy = particles[j].y - particles[i].y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < radius) {
                particles[i].neighbors.push_back(&particles[j]);
            }
        }
    }
}

// Compute cubic spline kernel function
float CubicSplineKernel(float distance, const float radius) {
    float q = distance / radius;
    float result = 0.0f;
    float alpha = 5 / (14 * M_PI * radius * radius);

    float t1 = std::max(0.0f, 1.0f - q);
    float t2 = std::max(0.0f, 2.0f - q);

    result = alpha * (t2 * t2 * t2 - 4.0f * t1 * t1 * t1);

    return result;
}

// Compute cubic spline kernel gradient
float CubicSplineKernelGradient(float distance, const float radius) {
    float q = distance / radius;
    float result = 0.0f;
    float alpha = 5 / (14 * M_PI * radius * radius);

    float t1 = std::max(0.0f, 1.0f - q);
    float t2 = std::max(0.0f, 2.0f - q);

    result = alpha * (3.0f * t2 * t2 - 12.0f * t1 * t1);

    return result;
}

// Test the kernel function and its gradient for uniformed grid of particles
void KernelTest(const float radius, const int numParticles) {
    for (int i = 0; i < particles.size(); i++) {
        float kernelSum = 0.0f;
        float kernelGradientSum = 0.0f;

        int sgn = 1;
        const float volumeRev = round(1 / (radius * radius) * 100) / 100;
        const float radiusRevNeg = round(-1 / radius * 100) / 100;

        if (particles[i].neighbors.size() < 13) {
            continue;
        }

        for (int j = 0; j < particles[i].neighbors.size(); j++) {
            float dX = particles[i].x - particles[i].neighbors[j]->x;
            float dY = particles[i].y - particles[i].neighbors[j]->y;

            float distance = std::sqrt(dX * dX + dY * dY);

            dX = round(dX * 100000) / 100000;
            if (dX > 0 || dX == 0 && dY > 0) {
                sgn = -1;
            }
            else {
                sgn = 1;
            }

            kernelSum += CubicSplineKernel(distance, radius);
            float temp = CubicSplineKernelGradient(distance, radius);
            kernelGradientSum += sgn * temp;
        }

        kernelSum = round(kernelSum * 100) / 100;
        kernelGradientSum = round(kernelGradientSum * 100) / 100;

        if (kernelSum - volumeRev >= kernelSum / 50) { // 2% error
            std::cout << "Kernel Sum test failed" << std::endl;
        }
        if (kernelGradientSum != 0) {
            std::cout << "Kernel Gradient Sum test failed" << std::endl;
        }
    }
}

// Loop through all particles and compute density
void ComputeDensity() {
    for (int i = 0; i < particles.size(); i++) {
        // Skip boundary particles
        if (particles[i].isFluid == false) {
            continue;
        }

        float density = 0.0f;
        for (int j = 0; j < particles[i].neighbors.size(); j++) {
            float dX = particles[i].x - particles[i].neighbors[j]->x;
            float dY = particles[i].y - particles[i].neighbors[j]->y;

            float distance = std::sqrt(dX * dX + dY * dY);

            density += particles[i].neighbors[j]->mass * CubicSplineKernel(distance, SPACING);
        }

        particles[i].density = density;
    }
}

// Loop through all particles and compute pressure
void ComputePressure(const float stiffness) {
    for (int i = 0; i < particles.size(); i++) {
        // Skip boundary particles
        if (particles[i].isFluid == false) {
            continue;
        }
        particles[i].pressure = std::max(0.0f, stiffness * (particles[i].density / REST_DENSITY - 1));
        if (particles[i].pressure != 0 && i != 136) {
            int a = 0;
        }
    }
}

// Loop through all particles and compute acceleration
void ComputeAcceleration(const float viscosity) {
    for (int i = 0; i < particles.size(); i++) {
        // Skip boundary particles
        if (particles[i].isFluid == false) {
            continue;
        }

        float aPressureX = 0.0f;
        float aPressureY = 0.0f;
        float aViscosityX = 0.0f;
        float aViscosityY = 0.0f;

        float aGravity = -9.81f;

        for (int j = 0; j < particles[i].neighbors.size(); j++) {
            // Compute distance between particles
            float dX = particles[i].x - particles[i].neighbors[j]->x;
            float dY = particles[i].y - particles[i].neighbors[j]->y;

            float distance = std::sqrt(dX * dX + dY * dY);

            float dVX = particles[i].vx - particles[i].neighbors[j]->vx;
            float dVY = particles[i].vy - particles[i].neighbors[j]->vy;

            float dV = std::sqrt(dVX * dVX + dVY * dVY);

            // Compute acceleration
            aViscosityX += particles[i].neighbors[j]->mass / particles[i].neighbors[j]->density *
                dVX * dX / (dX * dX + 0.01 * SPACING * SPACING) * CubicSplineKernelGradient(distance, SPACING);

            aViscosityY += particles[i].neighbors[j]->mass / particles[i].neighbors[j]->density *
                dVY * dY / (dY * dY + 0.01 * SPACING * SPACING) * CubicSplineKernelGradient(distance, SPACING);

            if (particles[i].neighbors[j]->isFluid == false) {
                aPressureX += dX / distance * particles[i].neighbors[j]->mass * (2 * particles[i].pressure /
                    (particles[i].density * particles[i].density)) * CubicSplineKernelGradient(distance, SPACING);
                aPressureY += dY / distance * particles[i].neighbors[j]->mass * (2 * particles[i].pressure /
                    (particles[i].density * particles[i].density)) * CubicSplineKernelGradient(distance, SPACING);
            }
            else {
                if (distance == 0) {
                    continue;
                }
                else {
                    aPressureX += dX / distance * particles[i].neighbors[j]->mass * (particles[i].pressure /
                        (particles[i].density * particles[i].density) + particles[i].neighbors[j]->pressure /
                        (particles[i].neighbors[j]->density * particles[i].neighbors[j]->density)) *
                        CubicSplineKernelGradient(distance, SPACING);
                    aPressureY += dY / distance * particles[i].neighbors[j]->mass * (particles[i].pressure /
                        (particles[i].density * particles[i].density) + particles[i].neighbors[j]->pressure /
                        (particles[i].neighbors[j]->density * particles[i].neighbors[j]->density)) *
                        CubicSplineKernelGradient(distance, SPACING);
                }
            }
        }

        // Update acceleration
        particles[i].ax = -2 * viscosity * aViscosityX + aPressureX;
        particles[i].ay = -2 * viscosity * aViscosityY + aPressureY + aGravity;
    }
}

// Euler integration
void UpdateParticles(const float dt) {
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i].isFluid == false) {
            continue;
        }
        particles[i].vx += particles[i].ax * dt;
        particles[i].vy += particles[i].ay * dt;
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
    }
}

void FallingSimulation() {
    // Choose the main parameters of the simulation
    float timestep = 0.0005f;
    const float stiffness = 1000.0f;
    const float viscosity = .1f;

    NeighborSearch(SUPPORT);
    ComputeDensity();
    ComputePressure(stiffness);
    ComputeAcceleration(viscosity);
    UpdateParticles(timestep);
}

void Setup() {
    //initialize particles
    InitBoundaries();
    InitParticles();

    for (int i = 0; i < particles.size(); i++) {
        // assign equal mass and rest density to all particles
        particles[i].mass = SPACING * SPACING / REST_DENSITY;
        particles[i].density = REST_DENSITY;
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }
    
    // Read parameters from the input file
    readParameters();

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SPH solver in 2D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);

    // Initialize particles
    Setup();

    // Load the provided shader program
    ShaderProgramSource source = ParseShader("Shaders/particle.vert", "Shaders/particle.frag");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    glUseProgram(shader);

    // Get the uniform location for the model-view-projection (MVP) matrix
    unsigned int u_MVP = glGetUniformLocation(shader, "modelViewProjection");

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Update particles
        FallingSimulation();

        glClear(GL_COLOR_BUFFER_BIT);

        // Set up the projection matrix
        glm::mat4 projection = glm::ortho(-1.0f, static_cast<float>(WINDOW_WIDTH), -1.0f, static_cast<float>(WINDOW_HEIGHT), -1.0f, 1.0f);

        // Render particles using the shader
        glUseProgram(shader); // Ensure the correct shader is active

        // Set a uniform to indicate whether a particle is a boundary
        int isBoundaryLocation = glGetUniformLocation(shader, "isBoundary");
        // Pass an additional uniform to indicate if a particle is a neighbor
        int isNeighborLocation = glGetUniformLocation(shader, "isNeighbor");

        for (const Particle& p : particles) {
            // Set the model matrix to translate the particle to its position
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, 0.0f));

            // Combine the model and projection matrices to form the MVP matrix
            glm::mat4 modelViewProjection = projection * model;

            // Set the MVP matrix as a uniform
            glUniformMatrix4fv(u_MVP, 1, GL_FALSE, &modelViewProjection[0][0]);
            glUniform1i(isBoundaryLocation, p.isFluid ? 0 : 1);

            int isNeighbor = 0; // Initialize isNeighbor flag

            //check if particle is a neighbor of a fluid particle number 25 
            for (int i = 0; i < particles[500].neighbors.size(); i++) {
                if (&p == particles[500].neighbors[i]) {
                    if (&p == &particles[500]) { isNeighbor = 2; } 
                    else { isNeighbor = 1; } // Set isNeighbor to 1 if it's a neighbor 
                    break;
                }
            }
            // Now, set the isNeighbor attribute for the current particle
            glUniform1i(isNeighborLocation, isNeighbor);

            int numSegments = 20; // Number of segments to approximate a circle
            float radius = 4.0f; // Radius of the circle

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(p.x, p.y); // Center of the circle

            for (int i = 0; i <= numSegments; ++i) {
                float angle = 2.0f * M_PI * static_cast<float>(i) / numSegments;
                float x = p.x + radius * cos(angle);
                float y = p.y + radius * sin(angle);
                glVertex2f(x, y);
            }

            glEnd();
        }

        glUseProgram(0); // Unbind the shader

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();

    return 0;
}
