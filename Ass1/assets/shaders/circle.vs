#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;

uniform mat4 transform;
uniform mat4 projection;

void main() {
    gl_Position = projection * transform * vec4(aPos, 0.0, 1.0);
    TexCoords = aPos + vec2(0.5, 0.5); // Convert from -0.5,0.5 to 0,1 range
}