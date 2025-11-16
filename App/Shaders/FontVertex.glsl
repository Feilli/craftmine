#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_Texture;

uniform mat4 u_Projection;

layout (location = 0) out vec2 TexCoords;

void main() {
    gl_Position = u_Projection * vec4(a_Position, 1.0);
    TexCoords = a_Texture;
}