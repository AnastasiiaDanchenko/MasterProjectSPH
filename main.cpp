#include "headers/Visualization.h"
#include "headers/Analysis.h"

// Close the window when pressing ESC
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
}

int main() {
    // Read parameters from the input file
    readParameters();

    if (VISUALIZATION) {
        Initialization(1);
        Visualize();
	} else {
        PerformanceAnalysisAll();
    }

    return 0;
}
