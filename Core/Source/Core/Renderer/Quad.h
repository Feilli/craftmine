#pragma once

#include "Renderer.h"

#include <glm/glm.hpp>

#include <stdint.h>

namespace Renderer {

    struct Transform {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);
    };

    class Quad {
    public:
        Quad();
        ~Quad();

        void SetShader(uint32_t shaderHandle);
        void SetTexture(uint32_t textureHandle);

        void SetVertices(Vertex vertices[4]);

        void SetPosition(glm::vec3 position);
        void SetScale(glm::vec3 scale);

        void InitGeometry();
        void Render(const glm::mat4& projection);

    private:
        uint32_t m_VertexArray = 0;
        uint32_t m_VertexBuffer = 0;
        uint32_t m_ElementBuffer = 0;
        uint32_t m_Shader = 0;

        uint32_t m_Texture = 0;

        Transform m_Transform;

        Vertex m_Vertices[4] = {
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } },
            { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } }
        };

        uint32_t m_Indices[6] = {
            0, 1, 2,
            2, 3, 0
        };
    };

}