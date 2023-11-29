#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "matplotlibcpp.h"
#include "headers/CompileShaders.h"
#include "headers/Simulation.h"

namespace plt = matplotlibcpp;

// Close the window when pressing ESC
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
}

void Visualization() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SPH solver in 2D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
       exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, keyCallback);

    ShaderProgramSource source = ParseShader("Shaders/particle.vert", "Shaders/particle.frag");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    glUseProgram(shader);

    // Get the uniform location for the model-view-projection (MVP) matrix
    unsigned int u_MVP = glGetUniformLocation(shader, "modelViewProjection");

    int count = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        if (NS_METHOD == "Verlet list") {
            if (count == 0) { UpdateVerletList(); }
            else if (count == 10) { count = -1; }
            count++;
        }

        // Update particles
        Simulation();

        glClear(GL_COLOR_BUFFER_BIT);

        // Set up the projection matrix
        glm::mat4 projection = glm::ortho(-1.0f, static_cast<float>(WINDOW_WIDTH), -1.0f, static_cast<float>(WINDOW_HEIGHT), -1.0f, 1.0f);
        glUseProgram(shader);

        // Set uniforms to indicate whether a particle is a boundary or/and a neighbor
        int isBoundaryLocation = glGetUniformLocation(shader, "isBoundary");
        int isNeighborLocation = glGetUniformLocation(shader, "isNeighbor");
        int isCandidateLocation = glGetUniformLocation(shader, "isCandidate");

        for (Particle& p : particles) {
            // Set the model matrix to translate the particle to its position
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(p.position.x(), p.position.y(), 0.0f));

            // Combine the model and projection matrices to form the MVP matrix
            glm::mat4 modelViewProjection = projection * model;

            // Set the MVP matrix as a uniform
            glUniformMatrix4fv(u_MVP, 1, GL_FALSE, &modelViewProjection[0][0]);
            glUniform1i(isBoundaryLocation, p.isFluid ? 0 : 1);

            int isNeighbor = 0;
            int isCandidate = 0;

            //check if particle is a neighbor of a fluid particle
            for (int i = 0; i < particles[PARTICLE_NEIGHBORS].neighbors.size(); i++) {
                if (&p == particles[PARTICLE_NEIGHBORS].neighbors[i]) {
                    if (&p == &particles[PARTICLE_NEIGHBORS]) { isNeighbor = 2; }
                    else { isNeighbor = 1; }
                    break;
                }
            }
            glUniform1i(isNeighborLocation, isNeighbor);

            //check if particle is a candidate of a fluid particle
            if (NS_METHOD == "Verlet list") {
                for (int i = 0; i < particles[PARTICLE_NEIGHBORS].candidates.size(); i++) {
                    if (&p == particles[PARTICLE_NEIGHBORS].candidates[i]) {
                        isCandidate = 1;
                        break;
                    }
                }
            }
            else if (NS_METHOD == "Uniform grid") {
                const int x = particles[PARTICLE_NEIGHBORS].getCellNumber().x();
                const int y = particles[PARTICLE_NEIGHBORS].getCellNumber().y();
                for (int i = x - 1; i <= x + 1; i++) {
                    for (int j = y - 1; j <= y + 1; j++) {
                        if (i < 0 || i >= GRID_WIDTH || j < 0 || j >= GRID_HEIGHT) {
                            continue;
                        }
                        for (auto& c : grid[i][j]) {
                            if (&p == c) {
                                isCandidate = 1;
                                break;
                            }
                        }
                    }
                }
            }
            glUniform1i(isCandidateLocation, isCandidate);

            int numSegments = 20;
            float radius = 4.0f;

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(p.position.x(), p.position.y());

            for (int i = 0; i <= numSegments; ++i) {
                float angle = 2.0f * M_PI * static_cast<float>(i) / numSegments;
                float x = p.position.x() + radius * cos(angle);
                float y = p.position.y() + radius * sin(angle);
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
}

void PerformanceAnalysis() {
    std::vector<std::string> NS_METHODS = { "Quadratic search", "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting" };
    SIMULATION = "Initial";

    std::vector<std::vector<double>> executionTimes;  // Matrix to store execution times for each method
    std::vector<int> nbParticles;
    std::vector<std::vector<int>> nbNeighbors;
    int steps = 1000;

    for (int i = 4; i <= 6; i += 2) {
        std::cout << "Number of fluid particles: " << PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION << std::endl;
        std::vector<double> timesForCurrentParticles;
        std::vector<int> neighborsForCurrentParticles;

        for (auto method : NS_METHODS) {
            int neighbors = 0;
            NS_METHOD = method;
            Initialization(i);
            int count = 0;

            auto start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < steps; j++) {
                if (NS_METHOD == "Verlet list") {
                    if (count == 0) { UpdateVerletList(); }
                    else if (count == 10) { count = -1; }
                    count++;
                }
                NeighborSearch();
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            timesForCurrentParticles.push_back(duration.count() / steps);
        }

        executionTimes.push_back(timesForCurrentParticles);
        WINDOW_WIDTH += PARTICLES_PER_DIMENSION * SUPPORT;
        nbParticles.push_back(PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION);
    }

    // Plot execution times against the number of particles
    plt::figure();
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
        std::vector<double> methodTimes;
        for (size_t j = 0; j < executionTimes.size(); ++j) {
            methodTimes.push_back(executionTimes[j][i]);
        }
        plt::plot(nbParticles, methodTimes);
        plt::named_plot(NS_METHODS[i], nbParticles, methodTimes);
    }

    plt::title("Performance analysis: Average computation time per simulation step");
    plt::xlabel("Number of particles");
    plt::ylabel("Time [s]");
    plt::legend();
    plt::show();
    plt::close();
}

void NeighborAnalysis() {
    std::vector<std::string> NS_METHODS = { "Quadratic search", "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting" };
    SIMULATION = "Initial";

    std::vector<int> nbParticles;
    std::vector<int> nbNeighbors;
    int steps = 1000;

    for (auto method : NS_METHODS) {
        int neighbors = 0;
        NS_METHOD = method;
        Initialization(4); // check for a big scene
        int count = 0;

        for (int j = 0; j < steps; j++) {
            if (NS_METHOD == "Verlet list") {
                if (count == 0) { UpdateVerletList(); }
                else if (count == 10) { count = -1; }
                count++;
            }
            NeighborSearch();
        }

        for (auto& p : particles) {
			neighbors += p.neighbors.size();
		}
        nbNeighbors.push_back(neighbors);
        particles.clear();
    }

    // Write statistics into console
    std::cout << "Total number of neighbors for " << NS_METHODS[0] << ": " << nbNeighbors[0] << std::endl << std::endl;
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
        std::cout << "Total number of neighbors for " << NS_METHODS[i] << ": " << nbNeighbors[i] << std::endl;
        std::cout << "Number of false positives for " << NS_METHODS[i] << ": " << nbNeighbors[i] - nbNeighbors[0] << std::endl << std::endl;
	}
    
}

int main() {
    // Read parameters from the input file
    readParameters();

    if (VISUALIZATION) {
        Initialization(1);
        Visualization();
	} else {
        //PerformanceAnalysis();
        NeighborAnalysis();
    }

    return 0;
}
