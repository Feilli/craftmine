#include "Camera.h"

#include "Core/Application.h"

#include <GLFW/glfw3.h>


#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {

}

Camera::~Camera() {
}

void Camera::Update(float deltaTime) {
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    
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

    if(ControlsActive[GLFW_KEY_W]) m_Position += m_Front * velocity;
    if(ControlsActive[GLFW_KEY_S]) m_Position -= m_Front * velocity;
    if(ControlsActive[GLFW_KEY_A]) m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * velocity;
    if(ControlsActive[GLFW_KEY_D]) m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * velocity;
    if(ControlsActive[GLFW_KEY_X]) m_Position -= m_Up * velocity;
    if(ControlsActive[GLFW_KEY_SPACE]) m_Position += m_Up * velocity;
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

float Camera::GetYaw() const {
    return m_Yaw;
}

void Camera::SetYaw(float angle) {
    m_Yaw = angle;
}

float Camera::GetPitch() const {
    return m_Pitch;
}

void Camera::SetPitch(float angle) {
    m_Pitch = angle;
}

glm::vec3 Camera::CastRay() {
    // cast ray from the camera center
    glm::vec2 cursorPos = Core::Application::Get().GetWindow()->GetCursorPos();
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