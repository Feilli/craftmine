#version 460 core

layout(location = 0) in vec3 a_Position;

out vec3 direction;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    direction = a_Position;

    mat4 rotationView = mat4(mat3(u_View));
    gl_Position = u_Projection * rotationView * vec4(a_Position, 1.0);
}