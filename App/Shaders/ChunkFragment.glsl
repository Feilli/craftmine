#version 460 core

in VS_OUT {
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
    float ao;
} fs_in;

uniform sampler2D u_Atlas;

uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform vec3 u_AmbientColor;

uniform vec3 u_CameraPosition;
uniform float u_FogStart;
uniform float u_FogEnd;

out vec4 FragColor;

void main() {
    vec4 textureColor = texture(u_Atlas, fs_in.uv);

    if(textureColor.a < 0.1) {
        discard;
    }

    // compute diffuse lighting
    float diffuse = max(dot(fs_in.normal, -u_SunDirection), 0.0);

    // combine diffuse and ambient
    vec3 lighting = u_AmbientColor + diffuse * u_SunColor;

    // apply AO as darkening factor
    float ao = fs_in.ao / 3.0;

    lighting *= ao;

    vec3 color = textureColor.rgb * lighting;

    // compute fog
    float dist = distance(u_CameraPosition, fs_in.fragPos);
    float fogFactor = clamp((dist - u_FogStart) / (u_FogEnd - u_FogStart), 0.0, 1.0);

    color = mix(color, u_AmbientColor, fogFactor);

    FragColor = vec4(color, textureColor.a);
} 