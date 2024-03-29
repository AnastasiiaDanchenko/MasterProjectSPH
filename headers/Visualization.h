#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CompileShaders.h"
#include "Simulation.h"

void keyCallbackVBO(GLFWwindow* window, int key, int scancode, int action, int mods);

void InitializeVBO();
void UpdateVBO();
void VisualizationVBO();

void Visualize();
