#include "HUDLayer.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Texture.h"
#include "Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HUDLayer::HUDLayer() :
    m_Font("Fonts/RobotoMono-Regular.ttf") {
    // load crosshair
    uint32_t crosshairShader = Renderer::CreateGraphicsShader("Shaders/HUDVertex.glsl", "Shaders/HUDFragment.glsl");

    int width, height;
    uint32_t crosshairTexture = Renderer::LoadTexture("Textures/crosshair.png", width, height);

    // init crosshair
    m_Crosshair.InitGeometry();

    m_Crosshair.SetShader(crosshairShader);
    m_Crosshair.SetTexture(crosshairTexture);

    m_Crosshair.SetScale(glm::vec3(0.1f, 0.1f, 0.0f));
}

HUDLayer::~HUDLayer() {
}

void HUDLayer::OnUpdate(float deltaTime) {
    // update debug info
    m_DebugInfo.FPS = Core::Application::Get().GetTickCount();
}

void HUDLayer::OnRender() {
    // Compute projection matrix
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    float aspect = frameBufferSize.x / frameBufferSize.y;

    glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    // Render Crosshair
    m_Crosshair.Render(projection);
    
    // Render Debug Info
    std::string fps = std::format("FPS: {}", m_DebugInfo.FPS);
    m_Font.RenderText(projection, fps, glm::vec2(-1.5f - 0.25f, 1.0f - 0.07f));

    std::string position = std::format("x: {:10f}, y: {:10f}, z: {:10f}", m_DebugInfo.Position.x, m_DebugInfo.Position.y, m_DebugInfo.Position.z);
    m_Font.RenderText(projection, position, glm::vec2(-1.5f - 0.25f, 1.0f - 0.14f));

    std::string blockHit = std::format("x: {:10f}, y: {:10f}, z: {:10f}", m_DebugInfo.BlockHit.x, m_DebugInfo.BlockHit.y, m_DebugInfo.BlockHit.z);
    m_Font.RenderText(projection, blockHit, glm::vec2(-1.5f - 0.25f, 1.0f - 0.21f));

    // Render current time
    float realSeconds = (m_DebugInfo.CurrentTime / m_DebugInfo.DayDuration) * 60 * 60 * 24;

    int hours = static_cast<int>(realSeconds) / 60 / 60;
    int minutes = (static_cast<int>(realSeconds) % 3600) / 60;

    std::string time = std::format("Time: {:02}:{:02}", hours, minutes);
    m_Font.RenderText(projection, time, glm::vec2(-1.5f - 0.25f, 1.0f - 0.28f));
}

void HUDLayer::OnPositionUpdatedEvent(glm::vec3 position) {
    m_DebugInfo.Position = position;
}

void HUDLayer::OnBlockHitUpdatedEvent(glm::vec3 position) {
    m_DebugInfo.BlockHit = position;
}

void HUDLayer::OnCurrenTimeUpdatedEvent(float currentTime, float dayDuration) {
    m_DebugInfo.CurrentTime = currentTime;
    m_DebugInfo.DayDuration = dayDuration;
}