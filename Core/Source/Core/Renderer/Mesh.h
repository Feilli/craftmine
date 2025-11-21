#pragma once

#include "Core/Renderer/Renderer.h"

#include <glm/glm.hpp>

#include <vector>

namespace Renderer {

    class Mesh {
    public:
        Mesh();
        ~Mesh();

        void Build(const std::vector<Vertex>& vertices,
                   const std::vector<uint32_t>& indices);
        void Bind();
        void Reset();

        int GetIndexCount();

    private:
        uint32_t m_VertexArray = 0;
        uint32_t m_VertexBufferVertices = 0;
        uint32_t m_VertexBufferUVs = 0;
        uint32_t m_ElementBuffer = 0;

        // uint32_t m_UniformBuffer = 0;

        int m_IndexCount = 0;
    };

}