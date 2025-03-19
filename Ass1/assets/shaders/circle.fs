#version 330 core
out vec4 FragColor;

uniform vec4 color;

void main() {
    // Calculate normalized position within the quad (0 to 1)
    vec2 pos = gl_PointCoord;
    pos = gl_FragCoord.xy / 100.0;
    pos = fract(pos);

    // Distance from center (0.5, 0.5)
    vec2 center = vec2(0.5, 0.5);
    float dist = length(pos - center) * 2.0;

    // If outside circle radius, discard the fragment
    if (dist > 1.0) {
        discard; // Makes it circular by not rendering outside the radius
    }

    // Add a lighting effect for 3D appearance
    float brightness = 1.0 - dist * 0.5;
    vec4 finalColor = color * brightness;

    // Output the final color
    FragColor = finalColor;
}