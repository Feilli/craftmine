#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <unordered_map>

class Camera {
public:
    Camera();
    ~Camera();

    void Update(float deltaTime);
    void UpdatePosition(float deltaTime);

    glm::vec3 GetPosition() const;
    glm::mat4 GetViewProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    float GetYaw() const;
    void SetYaw(float angle);

    float GetPitch() const;
    void SetPitch(float angle);

    glm::vec3 CastRay();
public:
    std::unordered_map<int, bool> ControlsActive = {
        { GLFW_KEY_W, false },
        { GLFW_KEY_S, false },
        { GLFW_KEY_A, false },
        { GLFW_KEY_D, false },
        { GLFW_KEY_X, false },
        { GLFW_KEY_SPACE, false }
    };
private:
    glm::vec3 m_Position = glm::vec3(0.0f, 64.0f, -3.0f);

    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

    float m_Yaw = -90.0f; // horizontal
    float m_Pitch = 0.0f; // vertical
    float m_FieldOfView = 45.0f;

    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_Speed = 12.0f;
    float m_MouseSensitivity = 0.1f;
};