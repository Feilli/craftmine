#pragma once

#include "Core/Layer.h"
#include "Core/Event.h"
#include "Core/Renderer/TextureAtlas.h"
#include "Core/Renderer/Shader.h"

#include "Camera.h"
#include "Lighting.h"
#include "ChunkManager.h"
#include "BoundingBox.h"

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>

class AppLayer : public Core::Layer {
public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;

    void OnKeyEventHandler(const Core::Event& event);
    void OnMouseButtonEventHandler(const Core::Event& event);
private:
    std::shared_ptr<Renderer::Shader> m_ChunkShader;
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;

    std::shared_ptr<ChunkManager> m_ChunkManager;

    int m_ViewDistance = 8;

    void UpdateChunks();
    void RenderChunks();

    void UpdateBlockOutline();
    void RenderBlockOutline();

    void UpdateSun(float deltaTime);

    glm::ivec2 WorldToChunkCoordinate(const glm::vec3& position);

    struct ChunkDistance { 
        glm::ivec2 Chunk;
        float Distance;
    };

    std::vector<ChunkDistance> m_ChunksSorted;

    struct BlockOutline {
        glm::vec2 Chunk;
        glm::vec3 Position;

        BoundingBox BoundingBox;

        bool Visible = false;
    };

    BlockOutline m_BlockOutline;
 
    Lighting::Sun m_Sun;

    // in seconds
    float m_CurrentTime = 0.0f;
    float m_DayDuration = 300.0f;

    Camera m_Camera;
};