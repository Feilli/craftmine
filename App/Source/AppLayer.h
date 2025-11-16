#pragma once

#include "Core/Layer.h"
#include "Core/Event.h"
#include "Core/Renderer/TextureAtlas.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Mesh.h"

#include "Camera.h"
#include "Block.h"
#include "BlockRegistry.h"
#include "Chunk.h"

#include <glm/glm.hpp>

#include <stdint.h>
#include <memory>

class AppLayer : public Core::Layer {
public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;

    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoord;
    };

    struct UVRect {
        // min
        float u0 = 0.0f; 
        float v0 = 0.0f;
        // max
        float u1 = 0.0f;
        float v1 = 0.0f;
    };

    struct AABB {
        glm::vec3 minBound;
        glm::vec3 maxBound;
    };

    struct Plane {
        glm::vec3 normal;
        float d;
    };

    struct Frustum {
        Plane planes[6];
    };

    Frustum GetFrustum(const glm::mat4 viewProjectionMatrix);
    bool AABBInFrustum(const Frustum& frustum, const AABB& box);

    void UpdateCameraRay();

    struct FaceHit {
        float T = 0.0f; // distance along the ray
        int Face = -1;
        glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& minBound, const glm::vec3& maxBound, float& tNear);
    bool RayIntersectsFace(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& model, FaceHit& hit);

    int GetSelectedBlockIndex();

    void OnKeyEventHandler(const Core::Event& event);
    void OnMouseButtonEventHandler(const Core::Event& event);

private:
    std::shared_ptr<Renderer::Shader> m_Shader;
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;

    BlockRegistry m_BlockRegistry;

    /*
        so if our chunk size is 16 * 16 = 256 blocks * 256 blocks height

        currently we are rendering 128 * 128 = 16386 * 256 blocks height

        in ideal world we need to render 16 * 16 * chunk render radius * 256 blocks height?
        I am not sure how many we can afford

        we can render chunks on the fly, because we have the seed + noise class

        let's try to first split our m_Blocks into chunks and render it
        let's as well make it so we are in the middle of our world and not on the edge
    */

    int m_ChunkSize = 16;
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

    std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>, ivec2_hash> m_ChunkMap;

    // std::vector<Block> m_Blocks;

    Camera m_Camera;
    // this can be a feature of the camera (like cast ray from camera)
    // and can accept ray distance and return a vector?
    glm::vec3 m_CameraRay;

    std::shared_ptr<Renderer::Mesh> CreateBlockMesh(std::unordered_map<BlockFace, Renderer::UVRect>& uvMap);
};