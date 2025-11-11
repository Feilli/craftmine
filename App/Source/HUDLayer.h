#pragma once

#include "Core/Layer.h"

#include <glm/glm.hpp>

#include <stdint.h>

class HUDLayer : public Core::Layer {
public:
    HUDLayer();
    virtual ~HUDLayer();

    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoord;
    };

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    
private:
    uint32_t m_VertexArray = 0;
    uint32_t m_VertexBuffer = 0;
    uint32_t m_ElementBuffer = 0;
    uint32_t m_UniformBuffer = 0;
    uint32_t m_Shader = 0;

    uint32_t m_Texture = 0;
};