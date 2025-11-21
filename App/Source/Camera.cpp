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

glm::vec3 Camera::GetPosition() const {
    return m_Position;
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
    return m_ProjectionMatrix * m_ViewMatrix;
}

glm::mat4 Camera::GetViewMatrix() const {
    return m_ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return m_ProjectionMatrix;
}

glm::vec3 Camera::CastRay() {
    // cast ray from the camera center
    glm::vec2 cursorPos = Core::Application::Get().GetCursorPos();
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();

    // normalize
    float x = (2.0f * cursorPos.x) / frameBufferSize.x - 1.0f;
    float y = 1.0f - (2.0f * cursorPos.y) / frameBufferSize.y;
    
    glm::vec2 normalizedCursorPos = glm::vec2(x, y);

    // convert normalize pos to world space
    glm::vec4 ray = glm::vec4(normalizedCursorPos.x, normalizedCursorPos.y, -1.0f, 1.0f);

    glm::vec4 rayProjection = glm::inverse(GetProjectionMatrix()) * ray;
    rayProjection.z = -1.0f;
    rayProjection.w = 0.0f;

    return glm::normalize(glm::vec3(glm::inverse(GetViewMatrix()) * rayProjection)); 
}