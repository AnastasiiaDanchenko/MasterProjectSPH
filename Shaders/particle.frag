#version 330 core

in vec4 color; // Input color from the vertex shader
uniform int isBoundary; // 0 if fluid, 1 if boundary
uniform int isNeighbor; // 2 if the particle itself, 1 if neighbor, 0 if not
uniform int isCandidate;
out vec4 fragColor; // Output fragment color

void main() {
    if (isBoundary == 1 && (isNeighbor == 1 || isCandidate == 1)) {
		fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for boundary neighbors
    } else if (isBoundary == 1) {
        fragColor = vec4(1.0, 0.5, 0.5, 1.0); // Light-Red color for boundary particles
    } else if (isNeighbor == 1) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green color for neighbor particles
    } else if (isNeighbor == 2) {
        fragColor = vec4(1.0, 1.0, 0.0, 1.0); // Yellow color for particle itself
    } else if (isCandidate == 1) {
        fragColor = vec4(0.0, 1.0, 1.0, 1.0); // Light-Blue color for candidate particles
    } else {
        fragColor = color; // Use the original color for fluid particles
    }
}
