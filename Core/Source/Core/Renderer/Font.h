#pragma once

#include "Shader.h"

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <filesystem>
#include <string>
#include <map>

namespace Renderer {

    struct Character {
        uint32_t Texture;

        glm::ivec2 Size;
        glm::ivec2 Bearing;

        uint32_t Advance;
    };

    class Font {
    public:
        Font(const std::filesystem::path& path);
        ~Font();

        void RenderText(const glm::mat4& projection, 
                        std::string text,
                        glm::vec2 position = glm::vec2(0.0f, 0.0f), 
                        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));

    private:
        uint32_t m_VertexArray = 0;
        uint32_t m_VertexBuffer = 0;
        uint32_t m_ElementBuffer = 0;
        uint32_t m_Shader = 0;

        int m_Size = 16;
        std::map<char, Character> m_Characters;
    };

}