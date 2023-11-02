#version 330 core

in vec4 color; // Input color from the vertex shader
uniform int isBoundary; // 0 if fluid, 1 if boundary
uniform int isNeighbor; // 2 if the particle itself, 1 if neighbor, 0 if not
out vec4 fragColor; // Output fragment color

void main() {
    if (isBoundary == 1 && isNeighbor == 1) {
		fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for boundary particles
	} else if (isBoundary == 1) {
        fragColor = vec4(1.0, 0.5, 0.5, 1.0); // Red color for boundary particles
    } else if (isNeighbor == 1) {
        fragColor = vec4(0.0, 1.0, 1.0, 1.0); // Blue color for neighbor particles
    } else if (isNeighbor == 2) {
        fragColor = vec4(1.0, 1.0, 0.0, 1.0); // Yellow color for particle itself
    } else {
        fragColor = color; // Use the original color for fluid particles
    }
}
