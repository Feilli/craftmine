#pragma once

#include "Core/Layer.h"
#include "Core/InputEvents.h"
#include "Core/Renderer/TextureAtlas.h"
#include "Core/Renderer/Shader.h"

#include "Camera.h"
#include "ChunkManager.h"
#include "BoundingBox.h"
#include "SkyBox.h"

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>

class AppLayer : public Core::Layer {
public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnEvent(Core::Event& event) override;

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
private:
    bool OnKeyPressed(const Core::KeyPressedEvent& event);
    bool OnKeyRelease(const Core::KeyReleasedEvent& event);
    bool OnMouseButtonPressed(const Core::MouseButtonPressedEvent& event);
    bool OnMouseMoved(const Core::MouseMovedEvent& event);
private:
    std::shared_ptr<Renderer::Shader> m_ChunkShader;
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;

    std::shared_ptr<ChunkManager> m_ChunkManager;

    int m_ViewDistance = 12;

    void SortChunks();
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
    SkyBox m_SkyBox;
    
    Camera m_Camera;
};