#pragma once

#include "Core/Layer.h"
#include "Core/Event.h"
#include "Core/Renderer/TextureAtlas.h"
#include "Core/Renderer/Shader.h"

#include "Camera.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "Intersects.h"
#include "BoundingBox.h"
#include "Perlin.h"

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>

class AppLayer : public Core::Layer {
public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;

    Intersects::Frustum GetFrustum(const glm::mat4 viewProjectionMatrix);

    struct BlockHit {
        glm::ivec2 Chunk;
        glm::ivec3 Block;

        bool Hit = false;
    };

    void OnKeyEventHandler(const Core::Event& event);
    void OnMouseButtonEventHandler(const Core::Event& event);

     BlockType GetWorldBlockType(glm::ivec3 position);
private:
    void UpdateChunks();

    std::shared_ptr<Renderer::Shader> m_ChunkShader;
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;

    std::shared_ptr<ChunkManager> m_ChunkManager;
    std::shared_ptr<Perlin> m_PerlinNoise;

    int m_ViewDistance = 8;

    glm::ivec2 WorldToChunkCoordinate(const glm::vec3& position);

    struct ivec2_hash {
        std::size_t operator()(const glm::ivec2& v) const noexcept {
            // Convert to 64-bit to avoid overflow
            uint64_t x = static_cast<uint32_t>(v.x);
            uint64_t y = static_cast<uint32_t>(v.y);

            uint64_t hash = x * 73856093ull ^y * 73856093ull;
            return std::hash<uint64_t>()(hash);
        }
    };

    // std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>, ivec2_hash> m_ChunkMap;

    struct ChunkDistance { 
        glm::ivec2 Chunk;
        float Distance;
    };

    std::vector<ChunkDistance> m_ChunksSorted;

    BlockHit m_BlockHit;
    BoundingBox m_BoundingBox;
    Camera m_Camera;
    
};