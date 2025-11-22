#include "Texture.h"

#include <iostream>
#include <print>

#include "stb_image.h"

namespace Renderer {

    Texture::Texture(const std::filesystem::path& path) {
        m_Handle = LoadTexture(path, m_Width, m_Height);
    }

    Texture::~Texture() {

    }

    void Texture::Bind() const {
        glBindTextureUnit(0, m_Handle);
    }

    uint32_t LoadTexture(const std::filesystem::path& path, int& width, int& height) {
        int channels;
        std::string filepath = path.string();

        unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

        if (!data) {
            std::cerr << "Failed to load texture: " << filepath << "\n";
            return {};
        }

        GLenum format = channels == 4 ? GL_RGBA :
            channels == 3 ? GL_RGB :
            channels == 1 ? GL_RED : 0;

        uint32_t result;

        glCreateTextures(GL_TEXTURE_2D, 1, &result);

        glTextureStorage2D(result, 1, (format == GL_RGBA ? GL_RGBA8 : GL_RGB8), width, height);

        glTextureSubImage2D(result, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

        // TODO: try one of these
        // glTextureParameteri(result.Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTextureParameteri(result, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(result, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(result, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(result, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateTextureMipmap(result);
        stbi_image_free(data);

        return result;
    }

}