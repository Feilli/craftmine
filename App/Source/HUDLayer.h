#pragma once

#include "Core/Layer.h"
#include "Core/Renderer/Quad.h"
#include "Core/Renderer/Font.h"

#include <glm/glm.hpp>

#include <stdint.h>

struct DebugInfo {
    int FPS = 0;
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 BlockHit = glm::vec3(0.0f);
};

class HUDLayer : public Core::Layer {
public:
    HUDLayer();
    virtual ~HUDLayer();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;

    void OnPositionUpdatedEvent(glm::vec3 position);
    void OnBlockHitUpdatedEvent(glm::vec3 position);

private:
    Renderer::Quad m_Crosshair;
    Renderer::Font m_Font;

    DebugInfo m_DebugInfo;
};