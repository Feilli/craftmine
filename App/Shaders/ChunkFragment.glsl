#version 460 core

in VS_OUT {
    vec2 uv;
    vec3 normal;
    float ao;
} fs_in;

uniform sampler2D u_Atlas;

uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform vec3 u_AmbientColor;

out vec4 FragColor;

void main() {
    vec4 textureColor = texture(u_Atlas, fs_in.uv);

    // if(textureColor.a < 0.1) {
    //     discard;
    // }

    // compute diffuse lighting
    float diffuse = max(dot(fs_in.normal, -u_SunDirection), 0.0);

    // combine diffuse and ambient
    vec3 lighting = u_AmbientColor + diffuse * u_SunColor;

    // apply AO as darkening factor
    float ao = fs_in.ao / 3.0;

    lighting *= ao;

    FragColor = vec4(textureColor.rgb * lighting, textureColor.a);
} 