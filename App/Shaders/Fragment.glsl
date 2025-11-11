#version 460 core

layout (location = 0) in vec2 TexCoords;

layout (binding = 0) uniform sampler2D u_Atlas;

uniform bool u_Highlight;

layout (location = 0) out vec4 FragColor;

void main() {
    if(u_Highlight) {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    } else {
        FragColor = texture(u_Atlas, TexCoords);
    }
} 