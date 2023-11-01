#version 330 core

layout(location = 0) in vec3 position; // Particle positions
out vec4 color; // Output color

uniform mat4 modelViewProjection; // Model-View-Projection matrix

void main() {
    gl_Position = modelViewProjection * vec4(position, 1.0);
    gl_PointSize = 1.0; // Adjust point size as needed
    color = vec4(0.3, 0.5, 1.0, 1.0); // Particle color (blue in RGBA)
}
