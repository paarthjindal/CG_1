#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec4 color;

void main() {
    // Calculate distance from center
    float dist = length(TexCoords - vec2(0.5, 0.5));

    // Simple circle with hard edges
    if (dist > 0.5) {
        discard;  // Outside of circle
    } else {
        // Just use the color as is for now
        FragColor = color;
    }
}