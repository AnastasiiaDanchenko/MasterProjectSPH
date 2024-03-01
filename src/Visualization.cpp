#include "../headers/Visualization.h"

const int IMGUI_WINDOW_WIDTH = 300;

bool isSimulationRunning = true;

typedef enum {
    POSITION_ATTRIBUTE = 0,
    COLOR_ATTRIBUTE,
    COUNT_ATTRIBUTES
} Attributes;

typedef struct {
    float x, y;
    float r, g, b;
} Vertex;

GLuint vao = 0;
GLuint vbo = 0;

#define VERTICES_CAPACITY 1000000
Vertex vertices[VERTICES_CAPACITY];
size_t verticesCount = 0;

// Close the window when pressing ESC
void keyCallbackVBO(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
}

void InitializeVBO() {
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * particles.size(), particles.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UpdateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * particles.size(), particles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VisualizationVBO(){
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
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

    glfwSetKeyCallback(window, keyCallbackVBO);

    ShaderProgramSource source = ParseShader("../Shaders/vbo.vert", "../Shaders/vbo.frag");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    // Initialize VBO
    InitializeVBO();

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

        // Update VBO
        UpdateVBO();

        glClear(GL_COLOR_BUFFER_BIT);

        // Set up the projection matrix
        glm::mat4 projection = glm::ortho(-1.0f, static_cast<float>(WINDOW_WIDTH), -1.0f, static_cast<float>(WINDOW_HEIGHT), -1.0f, 1.0f);
        glUseProgram(shader);

        // Set the MVP matrix as a uniform
        glUniformMatrix4fv(u_MVP, 1, GL_FALSE, &projection[0][0]);

        // Bind the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Rendering loop
        glDrawArrays(GL_TRIANGLE_FAN, 0, particles.size());

        // Unbind the VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUseProgram(0); // Unbind the shader

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up VBO
    glDeleteBuffers(1, &vbo);

    glDeleteProgram(shader);

    glfwTerminate();
}

void initBuffers() {
    // Create the vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Initialize the vertex attributes
    glEnableVertexAttribArray(POSITION_ATTRIBUTE);
    glVertexAttribPointer(POSITION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)offsetof(Vertex, x));
    glVertexAttribDivisor(POSITION_ATTRIBUTE, 1);

    glEnableVertexAttribArray(COLOR_ATTRIBUTE);
    glVertexAttribPointer(COLOR_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)offsetof(Vertex, r));
    glVertexAttribDivisor(COLOR_ATTRIBUTE, 1);
}

void syncBuffers() {
    glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices[0]) * verticesCount, vertices);
}

void pushVertex(float x, float y, float r, float g, float b) {
    if (verticesCount >= VERTICES_CAPACITY) {
		std::cerr << "Vertex buffer overflow!" << std::endl;
		exit(EXIT_FAILURE);
	}

	vertices[verticesCount].x = x;
	vertices[verticesCount].y = y;
	vertices[verticesCount].r = r;
	vertices[verticesCount].g = g;
	vertices[verticesCount].b = b;

	verticesCount++;
}

void clearBuffers() {
	verticesCount = 0;
}

void NSMethodButton(const char* label, const std::string& method) {
    if (ImGui::Button(label)) {
        particles.clear();
        NS_METHOD = method;
        Initialization(1);
    }
    ImGui::SameLine();
}

void Visualize() {
    // Initialize GLFW
    if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

    // Set the OpenGL version to 3.3 to use modern shaders
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(IMGUI_WINDOW_WIDTH + WINDOW_WIDTH, WINDOW_HEIGHT, "SPH solver in 2D", nullptr, nullptr);
    if (!window) {
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Intance drawing
    if (!GLEW_EXT_draw_instanced) {
		std::cerr << "Instanced drawing is not supported!" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the background color
    glClearColor(.05f, .05f, .05f, 1.0f);

    // Copile and link the shaders
    ShaderProgramSource source = ParseShader("Shaders/vbo.vert", "Shaders/vbo.frag");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    glUseProgram(shader);
    GLuint resolutionUniform = glGetUniformLocation(shader, "resolution");
    glUniform2f(resolutionUniform, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize the buffers
    initBuffers();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // step count for Verlet list
    int count = 0;

    // event loop
    while (!glfwWindowShouldClose(window)) {
        if (NS_METHOD == "Verlet list") {
			if (count == 0) { UpdateVerletList(); }
			else if (count == 3) { count = -1; } // 3 - CFL number
			count++;
        }
        else if (NS_METHOD == "Octree") {
            if (count == 0) { 
                GridUpdateIncremental();
                OctreeInit(Eigen::Vector2f(0.0f, 0.0f), Eigen::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT), 10); 
                SplitNode();
            }
			else if (count == 10) { count = -1; }
            count++;
        }

        if (isSimulationRunning) { Simulation(); }

        clearBuffers();

        // Render ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glViewport(IMGUI_WINDOW_WIDTH, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (NS_METHOD == "Index Sorting" || NS_METHOD == "Z-Index Sorting") {
            for (size_t i = 0; i < particles.size(); ++i) {
                Particle& p = particles[particleIndices[i]];
                if (!p.isFluid) { pushVertex(p.position.x(), p.position.y(), 1.0f, 0.7f, 0.5f); }
                else {
                    float minIndex = 0.0f;
                    float maxIndex = static_cast<float>(particles.size() - 1);

                    float normalizedIndex = (i - minIndex) / (maxIndex - minIndex);
                    float blue = 1.0f - normalizedIndex;

                    pushVertex(p.position.x(), p.position.y(), blue, blue, 1.0f);
                }
            }
        }
        else {
            for (auto& p : particles) {
                if (p.isFluid) {
                    bool isNeighbor = false;
                    for (auto& n : p.neighbors) {
                        if (n == &particles[PARTICLE_NEIGHBORS]) { isNeighbor = true; break; }
                    }
                    if (p.ID == PARTICLE_NEIGHBORS) { pushVertex(p.position.x(), p.position.y(), 1.0f, 1.0f, 0.0f); }
                    else if (isNeighbor) { pushVertex(p.position.x(), p.position.y(), 0.0f, 1.0f, 0.0f); }
                    else { pushVertex(p.position.x(), p.position.y(), 0.2f, 0.5f, 1.0f); }
                }
                else { pushVertex(p.position.x(), p.position.y(), 1.0f, 0.7f, 0.5f); }
            }
        }

        syncBuffers();

        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(7.0); // Set the point size
        glDrawArraysInstanced(GL_POINTS, 0, 1, verticesCount);

        //set fixed position for imgui window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(IMGUI_WINDOW_WIDTH, WINDOW_HEIGHT / 2));

        ImGui::Begin("Simulation Parameters");
        ImGui::Text("Number of particles: %d", particles.size());
        ImGui::Text("\nTime step:"); ImGui::SliderFloat("##TimeStep", &TIME_STEP, 0.0001f, 0.01f);
        ImGui::Text("\nStiffness coefficient:"); ImGui::SliderFloat("##Stiffness", &STIFFNESS, 1000.0f, 100000.0f);
        ImGui::Text("\nViscosity:"); ImGui::SliderFloat("##Viscosity", &VISCOSITY, 0.1f, 100.0f);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, WINDOW_HEIGHT / 2));
        ImGui::SetNextWindowSize(ImVec2(IMGUI_WINDOW_WIDTH, WINDOW_HEIGHT / 5));

        ImGui::Begin("Simulation Controls");
        if (ImGui::Button("Start/Pause")) { isSimulationRunning = !isSimulationRunning; }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) { particles.clear(); Initialization(1); }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, 7 * WINDOW_HEIGHT / 10));
        ImGui::SetNextWindowSize(ImVec2(IMGUI_WINDOW_WIDTH, 3 * WINDOW_HEIGHT / 10));

        ImGui::Begin("Neighborhood Search Method");
        NSMethodButton("Quadratic search", "Quadratic search");
        NSMethodButton("Verlet list", "Verlet list"); ImGui::NewLine();
        NSMethodButton("Uniform Grid", "Uniform grid");
        NSMethodButton("Spatial hashing", "Spatial hashing"); ImGui::NewLine();
        NSMethodButton("Index sorting", "Index Sorting");
        NSMethodButton("Z-Index Sorting", "Z-Index Sorting"); ImGui::NewLine();
        NSMethodButton("Octree", "Octree");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
	}

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(shader);
}
