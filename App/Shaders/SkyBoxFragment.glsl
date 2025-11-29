#version 460 core

in vec3 direction;

uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform vec3 u_AmbientColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(u_AmbientColor, 1.0);
}