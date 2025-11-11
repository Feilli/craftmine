#include "HUDLayer.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Texture.h"
#include "Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HUDLayer::HUDLayer() {
    // load shaders
    m_Shader = Renderer::CreateGraphicsShader("Shaders/HUDVertex.glsl", "Shaders/HUDFragment.glsl");

    // load texture
    int width, height;
    m_Texture = Renderer::LoadTexture("Textures/crosshair.png", width, height);

    // init geometry
    HUDLayer::Vertex vertices[] = {
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
    glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(HUDLayer::Vertex));

    // position attribute (location = 0)
    glEnableVertexArrayAttrib(m_VertexArray, 0);
    glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(HUDLayer::Vertex, Position)));
    glVertexArrayAttribBinding(m_VertexArray, 0, 0);

    // uv attribute (location = 1)
    glEnableVertexArrayAttrib(m_VertexArray, 1);
    glVertexArrayAttribFormat(m_VertexArray, 1, 2, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(HUDLayer::Vertex, TexCoord)));
    glVertexArrayAttribBinding(m_VertexArray, 1, 0);

    // bind the element buffer to the vertex array
    glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);
}

HUDLayer::~HUDLayer() {
    // clean up
    glDeleteVertexArrays(1, &m_VertexArray);
	glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_ElementBuffer);
    glDeleteBuffers(1, &m_UniformBuffer);

	glDeleteProgram(m_Shader);
}

void HUDLayer::OnUpdate(float deltaTime) {
    
}

void HUDLayer::OnRender() {
    // render crosshair
    glUseProgram(m_Shader);

    // scale crosshair
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    float aspect = frameBufferSize.x / frameBufferSize.y;

    glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.0f));
    
    GLuint modelLocation = glGetUniformLocation(m_Shader, "model");
    GLuint projectionLocation = glGetUniformLocation(m_Shader, "projection");

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glBindTextureUnit(0, m_Texture);
    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}