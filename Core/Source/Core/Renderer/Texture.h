#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <memory>

namespace Renderer {

    class Texture {
    public:
        Texture(const std::filesystem::path& path);
        ~Texture();

        void Bind() const;

    private:
        uint32_t m_Handle = 0;

        int m_Width = 0;
        int m_Height = 0;
    };

    // struct Texture {
    //     GLuint Handle = 0;
    //     uint32_t Width = 0;
    //     uint32_t Height = 0;
    // };

    uint32_t LoadTexture(const std::filesystem::path& path, int& width, int& height);

}