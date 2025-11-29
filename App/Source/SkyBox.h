#pragma once

#include "Camera.h"

#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Shader.h"

#include <glm/glm.hpp>

class SkyBox {
public:
    SkyBox();
    ~SkyBox();

    void Update(float deltaTime);
    void Render(const Camera& camera);

    glm::vec3 GetAmbientColor() const;
    glm::vec3 GetSunColor() const;
    glm::vec3 GetSunDirection() const;
private:
    Renderer::Mesh m_Mesh;
    Renderer::Shader m_Shader;

    glm::vec3 m_AmbientColor;
    glm::vec3 m_SunColor;
    glm::vec3 m_SunDirection;

    // in seconds
    float m_DayDuration = 60.0f;
    float m_CurrentTime = m_DayDuration / 2.0f;
};