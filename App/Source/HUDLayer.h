#pragma once

#include "Core/Layer.h"
#include "Core/Renderer/Quad.h"
#include "Core/Renderer/Font.h"

#include <glm/glm.hpp>

#include <stdint.h>

class HUDLayer : public Core::Layer {
public:
    HUDLayer();
    virtual ~HUDLayer();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    
private:
    Renderer::Quad m_Crosshair;
    Renderer::Font m_Font;
};