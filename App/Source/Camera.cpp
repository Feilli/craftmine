#include "Camera.h"

#include "Core/Application.h"

#include <GLFW/glfw3.h>


#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
}

Camera::~Camera() {
}

void Camera::Update(float deltaTime) {
    glm::vec2 cursorPos = Core::Application::Get().GetCursorPos();
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();

    m_Yaw -= m_MouseSensitivity * (frameBufferSize.x / 2 - cursorPos.x);
    m_Pitch += m_MouseSensitivity * (frameBufferSize.y / 2 - cursorPos.y);

    // clamp the pitch
    if(m_Pitch > 89.0f) m_Pitch = 89.0f;
    if(m_Pitch < -89.0f ) m_Pitch = -89.0f;

    // compute camera vectors
    m_Front = glm::normalize(glm::vec3(
        cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)),
        sin(glm::radians(m_Pitch)),
        sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch))
    ));

    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));

    // update position
    UpdatePosition(deltaTime);

    // update matrices
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), frameBufferSize.x / frameBufferSize.y, 0.1f, 1000.0f);
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::UpdatePosition(float deltaTime) {
    float velocity = m_Speed * deltaTime;

    // this is a dirty way to do it
    // I am not sure if dispatching an event is any better that this
    if(glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_W) == GLFW_PRESS) {
        m_Position += m_Front * velocity;
    } else if(glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_S) == GLFW_PRESS) {
        m_Position -= m_Front * velocity;
    } else if(glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_A) == GLFW_PRESS) {
        m_Position -= m_Right * velocity;
    } else if(glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_D) == GLFW_PRESS) {
        m_Position += m_Right * velocity;
    } else if(glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_Position += m_Up * velocity;
    } else if(glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_X) == GLFW_PRESS) {
        m_Position -= m_Up * velocity;
    }
}

glm::vec3 Camera::GetPosition() {
    return m_Position;
}

glm::mat4 Camera::GetViewProjectionMatrix() {
    return m_ProjectionMatrix * m_ViewMatrix;
}

glm::mat4 Camera::GetViewMatrix() {
    return m_ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() {
    return m_ProjectionMatrix;
}
