#pragma once

#include "Inventory.h"

#include "Core/Layer.h"
#include "Core/AppEvents.h"
#include "Core/InputEvents.h"
#include "Core/Renderer/Quad.h"
#include "Core/Renderer/Font.h"

#include <glm/glm.hpp>

#include <stdint.h>

struct DebugInfo {
    int FPS = 0;

    // PositionUpdated
    glm::vec3 Position = glm::vec3(0.0f);

    // TimeUpdated
    float DayTime = 0.0f;
    float DayDuration = 300.f;

    // ChunksGenerated
    int ChunksCreated = 0;
    float ChunksCreatedTime = 0.0f;
};

class HUDLayer : public Core::Layer {
public:
    HUDLayer();
    virtual ~HUDLayer();

    virtual void OnEvent(Core::Event& event) override;

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
private:
    void RenderDebugInfo();
    void RenderDebugInfoLine(std::string line);
private:
    bool OnPositionUpdatedEvent(const Core::PositionUpdatedEvent& event);
    bool OnTimeUpdatedEvent(const Core::TimeUpdatedEvent& event);
    bool OnChunksGeneratedEvent(const Core::ChunksGeneratedEvent& event);
    bool OnMouseScrollEvent(const Core::MouseScrollEvent& event);
private:
    Renderer::Quad m_Crosshair;
    Renderer::Font m_Font;

    glm::mat4 m_Projection;
    glm::vec2 m_NextLineOffset;

    DebugInfo m_DebugInfo;
    Inventory m_Inventory;
};