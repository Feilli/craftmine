#include "Quad.h"

#include <GLAD/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Renderer {

    Quad::Quad() {

    }

    Quad::~Quad() {
        glDeleteVertexArrays(1, &m_VertexArray);
        glDeleteBuffers(1, &m_VertexBuffer);
        glDeleteBuffers(1, &m_ElementBuffer);

        glDeleteProgram(m_Shader);
    }

    void Quad::SetShader(uint32_t shaderHandle) {
        m_Shader = shaderHandle;
    }

    void Quad::SetTexture(uint32_t textureHandle) {
        m_Texture = textureHandle;
    }

    void Quad::SetPosition(glm::vec3 position) {
        m_Transform.Position = position;
    }

    void Quad::SetScale(glm::vec3 scale) {
        m_Transform.Scale = scale;
    }

    void Quad::InitGeometry() {
        Vertex vertices[] = {
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } },
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // create arrays and buffers
        glCreateVertexArrays(1, &m_VertexArray);
        glCreateBuffers(1, &m_VertexBuffer);
        glCreateBuffers(1, &m_ElementBuffer);

        glNamedBufferData(m_VertexBuffer, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glNamedBufferData(m_ElementBuffer, sizeof(indices), indices, GL_STATIC_DRAW);

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

    void Quad::Render(const glm::mat4& projection) {
        // render quad
        glUseProgram(m_Shader);

        // scale quad
        glm::mat4 model = glm::mat4(1.0f);
        
        model = glm::scale(model, m_Transform.Scale);
        model = glm::translate(model, m_Transform.Position);

        GLuint modelLocation = glGetUniformLocation(m_Shader, "model");
        GLuint projectionLocation = glGetUniformLocation(m_Shader, "projection");

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glBindTextureUnit(0, m_Texture);
        glBindVertexArray(m_VertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

}