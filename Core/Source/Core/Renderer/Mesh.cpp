#include "Mesh.h"

#include <glad/gl.h>

namespace Renderer {

    Mesh::Mesh(const std::vector<glm::vec3>& vertices, 
                const std::vector<glm::vec2>& uvs,
                const std::vector<uint32_t> indices) {
        // create arrays and buffers
        glCreateVertexArrays(1, &m_VertexArray);
        glCreateBuffers(1, &m_VertexBufferVertices);
        glCreateBuffers(1, &m_VertexBufferUVs);
        glCreateBuffers(1, &m_ElementBuffer);

        glNamedBufferData(m_VertexBufferVertices, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
        glNamedBufferData(m_VertexBufferUVs, uvs.size() * sizeof(glm::vec2), &uvs.front(), GL_STATIC_DRAW);
        glNamedBufferData(m_ElementBuffer, indices.size() * sizeof(uint32_t), &indices.front(), GL_STATIC_DRAW);

        // bind vertices
        glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBufferVertices, 0, sizeof(glm::vec3));

        // position attribute (location = 0)
        glEnableVertexArrayAttrib(m_VertexArray, 0);
        glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_VertexArray, 0, 0);

        // bind UVs
        glVertexArrayVertexBuffer(m_VertexArray, 1, m_VertexBufferUVs, 0, sizeof(glm::vec2));

        // uv attribute (location = 1)
        glEnableVertexArrayAttrib(m_VertexArray, 1);
        glVertexArrayAttribFormat(m_VertexArray, 1, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_VertexArray, 1, 1);

        // bind the element buffer to the vertex array
        glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);

        m_IndexCount = indices.size();

        // create uniform buffer
        // glCreateBuffers(1, &m_UniformBuffer);
        // glNamedBufferData(m_UniformBuffer, sizeof(glm::mat4) * 2, nullptr, GL_DYNAMIC_DRAW);
        // glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UniformBuffer);
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &m_VertexArray);
        glDeleteBuffers(1, &m_VertexBufferVertices);
        glDeleteBuffers(1, &m_VertexBufferUVs);
        glDeleteBuffers(1, &m_ElementBuffer);
        // glDeleteBuffers(1, &m_UniformBuffer);
    }

    void Mesh::Bind() const {
        glBindVertexArray(m_VertexArray);
    }   

    int Mesh::GetIndexCount() const {
        return m_IndexCount;
    }

}