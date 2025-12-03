#include "SkyBox.h"

#include "Core/AppEvents.h"
#include "Core/Application.h"
#include "Core/Renderer/Renderer.h"

#include <glm/gtc/constants.hpp>
#include <glad/gl.h>

#include <vector>

SkyBox::SkyBox() :
    m_Shader("Shaders/SkyBoxVertex.glsl", "Shaders/SkyBoxFragment.glsl") {
    const std::vector<Renderer::Vertex> vertices = {
        // front
        { { -0.5f, -0.5f,  0.5f } },
        { {  0.5f, -0.5f,  0.5f } },
        { {  0.5f,  0.5f,  0.5f } },
        { { -0.5f,  0.5f,  0.5f } },
        // back
        { {  0.5f, -0.5f, -0.5f } },
        { { -0.5f, -0.5f, -0.5f } },
        { { -0.5f,  0.5f, -0.5f } },
        { {  0.5f,  0.5f, -0.5f } },
        // left
        { { -0.5f, -0.5f, -0.5f } },
        { { -0.5f, -0.5f,  0.5f } },
        { { -0.5f,  0.5f,  0.5f } },
        { { -0.5f,  0.5f, -0.5f } },
        // right
        { {  0.5f, -0.5f,  0.5f } },
        { {  0.5f, -0.5f, -0.5f } },
        { {  0.5f,  0.5f, -0.5f } },
        { {  0.5f,  0.5f,  0.5f } },
        // top
        { { -0.5f,  0.5f,  0.5f } },
        { {  0.5f,  0.5f,  0.5f } },
        { {  0.5f,  0.5f, -0.5f } },
        { { -0.5f,  0.5f, -0.5f } },
        // bottom
        { { -0.5f, -0.5f, -0.5f } },
        { {  0.5f, -0.5f, -0.5f } },
        { {  0.5f, -0.5f,  0.5f } },
        { { -0.5f, -0.5f,  0.5f } }
    };

    const std::vector<uint32_t> indices = {
        // front
         0,  1,  2,  2,  3,  0,
        // back
         4,  5,  6,  6,  7,  4,
        // left
         8,  9, 10, 10, 11,  8,
        // right
        12, 13, 14, 14, 15, 12,
        // top
        16, 17, 18, 18, 19, 16,
        // bottom
        20, 21, 22, 22, 23, 20
    };

    m_Mesh.Build(vertices, indices);
}

SkyBox::~SkyBox() {
    m_Mesh.Reset();
}

void SkyBox::Update(float deltaTime) {
    m_CurrentTime += deltaTime;
    
    if(m_CurrentTime > m_DayDuration) {
        m_CurrentTime = 0.0f;
    }

    float dayTime = m_CurrentTime / m_DayDuration;
    float sunAngle = dayTime * 2.0f * glm::pi<float>();

    // move sun on a hemisphere
    m_SunDirection = glm::normalize(glm::vec3(
        sin(sunAngle),
        -cos(sunAngle),
        0.0f
    ));

    // mix ambient color
    glm::vec3 dayAmbient  = glm::vec3(0.6f, 0.7f, 0.9f);
    glm::vec3 nightAmbient = glm::vec3(0.2f, 0.2f, 0.25f);

    m_AmbientColor = glm::mix(nightAmbient, dayAmbient, -cos(sunAngle) / 2.0f + 0.5f);
    
    // calculate sun color
    glm::vec3 sunColor = glm::vec3(1.0f, 0.95f, 0.8f);
    glm::vec3 moonColor = glm::vec3(0.3f, 0.25f, 0.2f);

    m_SunColor = glm::mix(moonColor, sunColor, -cos(sunAngle) / 2.0f + 0.5f);

    // push events
    Core::TimeUpdatedEvent event(m_CurrentTime, m_DayDuration);
    Core::Application::Get().RaiseEvent(event);
}

void SkyBox::Render(const Camera& camera) {
     // enable shader
    m_Shader.Use();

    // disable culling and deoth check
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glDisable(GL_CULL_FACE);

    // bind uniforms
    m_Shader.SetMat4("u_Projection", camera.GetProjectionMatrix());
    m_Shader.SetMat4("u_View", camera.GetViewMatrix());

    // bind lighting uniforms
    m_Shader.SetVec3("u_SunDirection", m_SunDirection);
    m_Shader.SetVec3("u_SunColor", m_SunColor);
    m_Shader.SetVec3("u_AmbientColor", m_AmbientColor);

    // bind texture atlas
    // m_TextureAtlas->GetTexture()->Bind();

    // bind water mesh
    m_Mesh.Bind();

    // draw water
    glDrawElements(GL_TRIANGLES, m_Mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);

    // enable culling and depth
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

glm::vec3 SkyBox::GetAmbientColor() const {
    return m_AmbientColor;
}

glm::vec3 SkyBox::GetSunColor() const {
    return m_SunColor;
}

glm::vec3 SkyBox::GetSunDirection() const {
    return m_SunDirection;
}