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
    
}

void HUDLayer::OnRender() {
    // Compute projection matrix
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    float aspect = frameBufferSize.x / frameBufferSize.y;

    glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    // Render Crosshair
    m_Crosshair.Render(projection);
    
    // Render Debug Info
    std::string fps = std::format("FPS: {}", Core::Application::Get().GetTickCount());
    m_Font.RenderText(projection, fps, glm::vec2(-1.5f - 0.25f, 1.0f - 0.07f));
}
