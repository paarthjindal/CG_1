#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform vec4 color;

void main() {
    // Use the texture coordinates directly for circle calculation
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoords, center);

    // Create a circle with smooth edges
    if (dist > 0.45) {
        discard; // Outside the circle radius
    }

    // Add 3D lighting effect
    float brightness = 1.0 - (dist * 1.5);
    float reflection = pow(1.0 - dist, 3.0) * 0.4;
    brightness += reflection;
    brightness = max(brightness, 0.3);

    FragColor = color * brightness;
}