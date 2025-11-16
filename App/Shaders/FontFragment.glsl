#version 460 core

layout (location = 0) in vec2 TexCoords;

layout (binding = 0) uniform sampler2D u_Texture;

uniform vec3 u_Color;

layout (location = 0) out vec4 FragColor;

void main() {
    float alpha = texture(u_Texture, TexCoords).r;
    FragColor = vec4(u_Color, alpha);
} 