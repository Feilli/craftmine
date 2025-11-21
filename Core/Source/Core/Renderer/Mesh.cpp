#include "Mesh.h"

#include <glad/gl.h>

namespace Renderer {

    Mesh::Mesh() {
        
    }

    Mesh::~Mesh() {
        Reset();
    }

    void Mesh::Build(const std::vector<Vertex>& vertices, 
                     const std::vector<uint32_t>& indices) {
        // create arrays and buffers
        glCreateVertexArrays(1, &m_VertexArray);
        glCreateBuffers(1, &m_VertexBufferVertices);
        //glCreateBuffers(1, &m_VertexBufferUVs);
        glCreateBuffers(1, &m_ElementBuffer);

        // TODO: change the VAO creation to upload vertex
        glNamedBufferData(m_VertexBufferVertices, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        // glNamedBufferData(m_VertexBufferUVs, uvs.size() * sizeof(glm::vec2), &uvs.front(), GL_STATIC_DRAW);
        glNamedBufferData(m_ElementBuffer, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        // bind vertices
        glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBufferVertices, 0, sizeof(Vertex));

        // position attribute (location = 0)
        glEnableVertexArrayAttrib(m_VertexArray, 0);
        glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
        
        glVertexArrayAttribBinding(m_VertexArray, 0, 0);

        // bind UVs
        // glVertexArrayVertexBuffer(m_VertexArray, 1, m_VertexBufferUVs, 0, sizeof(glm::vec2));

        // uv attribute (location = 1)
        glEnableVertexArrayAttrib(m_VertexArray, 1);
        glVertexArrayAttribFormat(m_VertexArray, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, UVs));
        glVertexArrayAttribBinding(m_VertexArray, 1, 0);

        // bind the element buffer to the vertex array
        glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);

        // create uniform buffer
        // glCreateBuffers(1, &m_UniformBuffer);
        // glNamedBufferData(m_UniformBuffer, sizeof(glm::mat4) * 2, nullptr, GL_DYNAMIC_DRAW);
        // glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UniformBuffer);

        m_IndexCount = static_cast<int>(indices.size());
    }

    void Mesh::Bind() {
        glBindVertexArray(m_VertexArray);
    }

    void Mesh::Reset() {
        glDeleteVertexArrays(1, &m_VertexArray);
        glDeleteBuffers(1, &m_VertexBufferVertices);
        //glDeleteBuffers(1, &m_VertexBufferUVs);
        glDeleteBuffers(1, &m_ElementBuffer);
        // glDeleteBuffers(1, &m_UniformBuffer);
    }

    int Mesh::GetIndexCount() {
        return m_IndexCount;
    }

}