#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_Texture;

// layout (binding = 0) uniform Matrices {
//     mat4 u_ViewProjection;
//     mat4 u_Transform;
// };

uniform mat4 u_Projection;
uniform mat4 u_View;

layout (location = 0) out vec2 TexCoords;

void main() {
    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
    TexCoords = a_Texture;
}
