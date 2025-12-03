#version 460 core

layout (location = 0) in vec2 TexCoords;

layout (binding = 0) uniform sampler2D u_Texture;

layout (location = 0) out vec4 FragColor;

void main() {
    vec4 textureColor = texture(u_Texture, TexCoords);

    if(textureColor.a < 0.1)
        discard;

    FragColor = textureColor;
} 