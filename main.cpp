#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "matplotlibcpp.h"

#include "headers/CompileShaders.h"
#include "headers/Simulation.h"

// Close the window when pressing ESC
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
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

    Initialization();

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

        for (const Particle& p : particles) {
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
            } else if (NS_METHOD == "Uniform grid") {
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

    return 0;
}
