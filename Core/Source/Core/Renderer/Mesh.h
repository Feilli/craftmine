#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Renderer {

    class Mesh {
    public:
        Mesh(const std::vector<glm::vec3>& vertices, 
            const std::vector<glm::vec2>& uvs,
            const std::vector<uint32_t> indices);

        ~Mesh();

        void Bind() const;
        int GetIndexCount() const;

    private:
        uint32_t m_VertexArray = 0;
        uint32_t m_VertexBufferVertices = 0;
        uint32_t m_VertexBufferUVs = 0;
        uint32_t m_ElementBuffer = 0;
        // uint32_t m_UniformBuffer = 0;

        int m_IndexCount = 0;
    };

}