#include "Font.h"

#include "Renderer.h"

#include <GLAD/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <assert.h>

namespace Renderer {
    
    Font::Font(const std::filesystem::path& path) {
        // load font
        FT_Library ft;
        FT_Init_FreeType(&ft);
        
        if(!ft) {
            assert(false);
            std::cerr << "Could not init FreeType library" << std::endl;
        }
    
        std::string filepath = path.string();
        FT_Face face;

        FT_New_Face(ft, filepath.c_str(), 0, &face);
        
        if(!face) {
            assert(false);
            std::cerr << "Could not load font: " << filepath << std::endl;
        }

        FT_Set_Pixel_Sizes(face, 0, m_Size);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for(GLubyte c = 0; c < 128; c++) {
            if(FT_Load_Char(face, c, FT_LOAD_RENDER))
                continue;

            // space
            if(face->glyph->bitmap.width == 0 || face->glyph->bitmap.rows == 0) {
                Character character = {
                    0,
                    glm::ivec2(0, 0),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<uint32_t>(face->glyph->advance.x)
                };

                m_Characters.insert(std::pair<char, Character>(c, character));

                continue;    
            }

            uint32_t texture;

            glCreateTextures(GL_TEXTURE_2D, 1, &texture);

            glTextureStorage2D(texture, 1, GL_R8, face->glyph->bitmap.width, face->glyph->bitmap.rows);

            glTextureSubImage2D(texture, 0, 0, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<uint32_t>(face->glyph->advance.x)
            };

            m_Characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // load shaders
        m_Shader = CreateGraphicsShader("Shaders/FontVertex.glsl", "Shaders/FontFragment.glsl");

        // create buffers for our geometry
        glCreateVertexArrays(1, &m_VertexArray);
        glCreateBuffers(1, &m_VertexBuffer);
        glCreateBuffers(1, &m_ElementBuffer);

        glNamedBufferData(m_VertexBuffer, sizeof(Vertex) * 6, nullptr, GL_DYNAMIC_DRAW);
        glNamedBufferData(m_ElementBuffer, sizeof(uint32_t) * 6, nullptr, GL_DYNAMIC_DRAW);

        // bind VBO and VAO to binding index 0
        glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(Vertex));

        // position attribute (location = 0)
        glEnableVertexArrayAttrib(m_VertexArray, 0);
        glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, Position)));
        glVertexArrayAttribBinding(m_VertexArray, 0, 0);

        // uv attribute (location = 1)
        glEnableVertexArrayAttrib(m_VertexArray, 1);
        glVertexArrayAttribFormat(m_VertexArray, 1, 2, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, UVs)));
        glVertexArrayAttribBinding(m_VertexArray, 1, 0);

        // bind the element buffer to the vertex array
        glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);
    }

    Font::~Font() {

    }

    void Font::RenderText(const glm::mat4& projection, std::string text, glm::vec2 position, glm::vec3 color) {
        // render quad
        glUseProgram(m_Shader);

        uint32_t projectionLocation = glGetUniformLocation(m_Shader, "u_Projection");
        uint32_t colorLocation = glGetUniformLocation(m_Shader, "u_Color");

        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(colorLocation, color.x, color.y, color.z);

        glBindVertexArray(m_VertexArray);

        for(const char& c: text) {
            Character ch = m_Characters[c];

            float scale = 1.0f;

            glm::vec2 pos = glm::vec2(
                position.x + ch.Bearing.x * scale, 
                position.y - (ch.Size.y - ch.Bearing.y) * scale);
            
            float width = ch.Size.x * scale;
            float height = ch.Size.y * scale;

            Vertex vertices[] = {
                { { pos.x        , pos.y + height, 0.0f }, { 0.0f, 0.0f } },
                { { pos.x        , pos.y         , 0.0f }, { 0.0f, 1.0f } },
                { { pos.x + width, pos.y         , 0.0f }, { 1.0f, 1.0f } },

                { { pos.x        , pos.y + height, 0.0f }, { 0.0f, 0.0f } },
                { { pos.x + width, pos.y         , 0.0f }, { 1.0f, 1.0f } },
                { { pos.x + width, pos.y + height, 0.0f }, { 1.0f, 0.0f } },
            };

            uint32_t indices[] = {
                0, 1, 2,
                3, 4, 5
            };

            // update vertex buffer
            glNamedBufferSubData(m_VertexBuffer, 0, sizeof(vertices), vertices);
            glNamedBufferSubData(m_ElementBuffer, 0, sizeof(indices), indices);

            glBindTextureUnit(0, ch.Texture);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // shift character
            position.x += (ch.Advance >> 6) * scale;
        }
    }

}