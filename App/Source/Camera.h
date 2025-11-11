#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    ~Camera();

    void Update(float deltaTime);
    void UpdatePosition(float deltaTime);

    glm::vec3 GetPosition();
    glm::mat4 GetViewProjectionMatrix();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

private:
    glm::vec3 m_Position = glm::vec3(0.0f, 3.0f, -3.0f);

    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

    float m_Yaw = 0.0f; // horizontal
    float m_Pitch = 0.0f; // vertical
    float m_FieldOfView = 45.0f;

    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_Speed = 3.0f;
    float m_MouseSensitivity = 0.1f;
};