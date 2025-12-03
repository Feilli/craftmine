#include "HUDLayer.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Texture.h"
#include "Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <print>

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

    glm::vec2 scale(32.0f, 32.0f);
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();

    m_Crosshair.SetScale(glm::vec3(32.0f, 32.0f, 0.0f));
    m_Crosshair.SetPosition(glm::vec3(frameBufferSize.x / 2, frameBufferSize.y / 2, 0.0f));
}

HUDLayer::~HUDLayer() {
}

void HUDLayer::OnEvent(Core::Event& event) {
    Core::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<Core::PositionUpdatedEvent>([this](Core::PositionUpdatedEvent& e) { return OnPositionUpdatedEvent(e); });
    dispatcher.Dispatch<Core::TimeUpdatedEvent>([this](Core::TimeUpdatedEvent& e) { return OnTimeUpdatedEvent(e); });
    dispatcher.Dispatch<Core::ChunksGeneratedEvent>([this](Core::ChunksGeneratedEvent& e) { return OnChunksGeneratedEvent(e); });
    dispatcher.Dispatch<Core::MouseScrollEvent>([this](Core::MouseScrollEvent& e) { return OnMouseScrollEvent(e); });
}

void HUDLayer::OnUpdate(float deltaTime) {
    // update debug info
    m_DebugInfo.FPS = Core::Application::Get().GetTickCount();

    // update projection matrix
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    float aspect = frameBufferSize.x / frameBufferSize.y;

    // m_Projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    m_Projection = glm::ortho(0.0f, frameBufferSize.x, 0.0f, frameBufferSize.y);

    m_Inventory.Update();
}

void HUDLayer::OnRender() {
    // Render Inventory
    m_Inventory.Render(m_Projection);

    // Render Crosshair
    m_Crosshair.Render(m_Projection);
    
    // Render Debug Info
    RenderDebugInfo();
}

void HUDLayer::RenderDebugInfo() {
    // Reset new line offset
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    m_NextLineOffset = { 10.0f, frameBufferSize.y - 26.0f };

    // FPS
    RenderDebugInfoLine(std::format("FPS: {}", m_DebugInfo.FPS));

    // Position
    RenderDebugInfoLine(std::format("x:{:10f}, y:{:10f}, z:{:10f}", m_DebugInfo.Position.x, m_DebugInfo.Position.y, m_DebugInfo.Position.z));

    // Time
    float realSeconds = (m_DebugInfo.DayTime / m_DebugInfo.DayDuration) * 60 * 60 * 24;

    int hours = static_cast<int>(realSeconds) / 60 / 60;
    int minutes = (static_cast<int>(realSeconds) % 3600) / 60;

    RenderDebugInfoLine(std::format("Day Time: {:02}:{:02}", hours, minutes));

    // Chunks
    RenderDebugInfoLine(std::format("Created {} chunks in {} seconds.", m_DebugInfo.ChunksCreated, m_DebugInfo.ChunksCreatedTime));
}

void HUDLayer::RenderDebugInfoLine(std::string line) {
    m_Font.RenderText(m_Projection, line, m_NextLineOffset);
    m_NextLineOffset -= glm::vec2(0.0f, 20.0f);
}

bool HUDLayer::OnPositionUpdatedEvent(const Core::PositionUpdatedEvent& event) {
    m_DebugInfo.Position = event.GetPosition();

    return false;
}

bool HUDLayer::OnTimeUpdatedEvent(const Core::TimeUpdatedEvent& event) {
    m_DebugInfo.DayTime = event.GetDayTime();
    m_DebugInfo.DayDuration = event.GetDayDuration();

    return false;
}

bool HUDLayer::OnChunksGeneratedEvent(const Core::ChunksGeneratedEvent& event) {
    m_DebugInfo.ChunksCreated = event.GetChunks();
    m_DebugInfo.ChunksCreatedTime = event.GetTime();

    return false;
}

bool HUDLayer::OnMouseScrollEvent(const Core::MouseScrollEvent& event) {
    m_Inventory.SetSelectedItem(event.GetYOffset());
    
    return false;
}