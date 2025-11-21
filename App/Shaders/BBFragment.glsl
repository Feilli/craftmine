#version 460 core

// layout (location = 0) in vec2 TexCoords;

// layout (binding = 0) uniform sampler2D u_Atlas;

layout (location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
} 