#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_UV;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in float a_AO;

// layout (binding = 0) uniform Matrices {
//     mat4 u_ViewProjection;
//     mat4 u_Transform;
// };

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out VS_OUT {
    vec2 uv;
    vec3 normal;
    float ao;
} vs_out;

void main() {
    vs_out.uv = a_UV;
    vs_out.normal = a_Normal;
    vs_out.ao = a_AO;

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
