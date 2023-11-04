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

    //initialize particles
    InitBoundaries();
    InitFluid();
    if (NS_METHOD != "Verlet list" && NS_METHOD != "Quadratic search") {
        UniformGrid();
    }

    // Load the provided shader program
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

        // Render particles using the shader
        glUseProgram(shader); // Ensure the correct shader is active

        // Set a uniform to indicate whether a particle is a boundary
        int isBoundaryLocation = glGetUniformLocation(shader, "isBoundary");
        // Pass an additional uniform to indicate if a particle is a neighbor
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

            int isNeighbor = 0; // Initialize isNeighbor flag
            int isCandidate = 0; // Initialize isCandidate flag

            //check if particle is a neighbor of a fluid particle
            for (int i = 0; i < particles[PARTICLE_NEIGHBORS].neighbors.size(); i++) {
                if (&p == particles[PARTICLE_NEIGHBORS].neighbors[i]) {
                    if (&p == &particles[PARTICLE_NEIGHBORS]) { isNeighbor = 2; }
                    else { isNeighbor = 1; } // Set isNeighbor to 1 if it's a neighbor 
                    break;
                }
            }
            glUniform1i(isNeighborLocation, isNeighbor);

            //check if particle is a candidate of a fluid particle
            if (NS_METHOD == "Verlet list") {
                for (int i = 0; i < particles[PARTICLE_NEIGHBORS].candidates.size(); i++) {
                    if (&p == particles[PARTICLE_NEIGHBORS].candidates[i]) {
                        isCandidate = 1; // Set isCandidate to 1 if it's a candidate
                        break;
                    }
                }
            } else if (NS_METHOD != "Quadratic search") {

            }
            glUniform1i(isCandidateLocation, isCandidate);        

            int numSegments = 20; // Number of segments to approximate a circle
            float radius = 4.0f; // Radius of the circle

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(p.position.x(), p.position.y()); // Center of the circle

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
