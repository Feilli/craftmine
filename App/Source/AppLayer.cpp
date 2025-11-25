#include "AppLayer.h"

#include "Perlin.h"

#include "Core/Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <print>
#include <numeric>
#include <ranges>

AppLayer::AppLayer() {
    // load shaders
    m_ChunkShader = std::make_shared<Renderer::Shader>("Shaders/ChunkVertex.glsl", "Shaders/ChunkFragment.glsl");

    // load texture
    m_TextureAtlas = std::make_shared<Renderer::TextureAtlas>("Textures/terrain.png", 16, 16);
    
    // create perling noise obejct
    m_PerlinNoise = std::make_shared<Perlin>(1234567890);

    // create chunk manager
    m_ChunkManager = std::make_shared<ChunkManager>(m_TextureAtlas, m_ChunkShader, m_PerlinNoise);
    
    m_ChunksSorted.reserve(m_ViewDistance * m_ViewDistance);

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // enable texture blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glFrontFace(GL_CCW);

    // subscribe for mouse and keyboard events
    // I think there is a smarter way to sub for those events
    Core::Application::Get().GetEventDispatcher()->AddListener([this](const Core::Event& event) {
        switch(event.Type) {
            case Core::EventType::KeyPressed:
                this->OnKeyEventHandler(event);
                break;
            case Core::EventType::MouseButtonPressed:
            case Core::EventType::MouseButtonReleased:
                this->OnMouseButtonEventHandler(event);
                break;
        }
    });
}

AppLayer::~AppLayer() {
}

void AppLayer::OnUpdate(float deltaTime) {
    // update camera
    m_Camera.Update(deltaTime);
    glm::vec3 cameraRay = m_Camera.CastRay();

    UpdateChunks();

    // sort chunks from camera
    m_ChunksSorted.clear();

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        Intersects::AABB chunkBoundingBox = chunk->second->GetBoundBox();
        
        glm::vec2 minBound = { chunkBoundingBox.MinBound.x, chunkBoundingBox.MinBound.z };
        glm::vec2 maxBound = { chunkBoundingBox.MaxBound.x, chunkBoundingBox.MaxBound.z };

        glm::vec2 center = (minBound + maxBound) * 0.5f;
        glm::vec2 cameraPosition = { m_Camera.GetPosition().x, m_Camera.GetPosition().z };

        // calculate squared distance between center of the chunk and camera
        // we can actually cache it on chunks itself
        glm::vec2 diff = center - cameraPosition;
        float distance = glm::dot(diff, diff);

        m_ChunksSorted.push_back({chunk->first, distance});
    }

    std::sort(m_ChunksSorted.begin(), m_ChunksSorted.end(),
              [](auto& a, auto& b){ return a.Distance < b.Distance; });

    // set chunk visibility
    Intersects::Frustum cameraFrustum = GetFrustum(m_Camera.GetViewProjectionMatrix());

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        if(Intersects::AABBFrustum(cameraFrustum, chunk->second->GetBoundBox())) {
            chunk->second->Visible = true;
        } else {
            chunk->second->Visible = false;
        }
    }

    // hightlight block
    float closestDistance = Chunk::s_ChunkSize; // maximum distance from which we pick up an object

    m_BlockHit.Hit = false;

    const float viewDistanceSquared = Chunk::s_ChunkSize * Chunk::s_ChunkSize * 4;

    // for(const auto& chunk : hitCandidates) {
    for(const auto& chunk : m_ChunksSorted) {
        if(!m_ChunkManager->GetChunk(chunk.Chunk)->Visible) {
            continue;
        }

        if(chunk.Distance > viewDistanceSquared) {
            continue;
        }

        for(const auto& block : m_ChunkManager->GetChunk(chunk.Chunk)->BlockVisible) {
            BlockType type = m_ChunkManager->GetChunk(chunk.Chunk)->GetBlockType(block);

            if(type == BlockType::AIR || type == BlockType::WATER) {
                continue;
            }

            glm::vec3 position = m_ChunkManager->GetChunk(chunk.Chunk)->GetBlockPosition(block);

            Intersects::AABB boundBox = {
                position - glm::vec3(0.5f),
                position + glm::vec3(0.5f)
            };

            float distance;
            bool highlight = Intersects::RayAABB(m_Camera.GetPosition(), cameraRay, boundBox, distance);

            if(highlight && distance < closestDistance) {
                closestDistance = distance;

                m_BlockHit.Hit = true;

                m_BlockHit.Chunk = chunk.Chunk;
                m_BlockHit.Block = block;
            }
        }

        if(m_BlockHit.Hit) {
            break;
        }
    }

    if(m_BlockHit.Hit) {
        glm::vec3 position = m_ChunkManager->GetChunk(m_BlockHit.Chunk)->GetBlockPosition(m_BlockHit.Block);

        Intersects::AABB boundBox = {
            -glm::vec3(0.5f),
             glm::vec3(0.5f)
        };

        m_BoundingBox.SetBoundingBox(boundBox);
        m_BoundingBox.SetPosition(position);
        m_BoundingBox.Update();

        Core::Event blockHitUpdatedEvent = { 
            .Type = Core::EventType::BlockHitUpdated, 
            .Position = position
        };

        Core::Application::Get().GetEventDispatcher()->PushEvent(blockHitUpdatedEvent);
    }

    // push events
    Core::Event positionUpdatedEvent = { 
        .Type = Core::EventType::PositionUpdated, 
        .Position = m_Camera.GetPosition()  
    };

    Core::Application::Get().GetEventDispatcher()->PushEvent(positionUpdatedEvent);
}

void AppLayer::OnRender() {
    // rendering commands
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    int chunksRendered = 0;

    // render terrain
    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        if(chunk->second->Visible) {
            chunk->second->RenderOpaqueMesh(m_Camera);
            chunksRendered++;
        }
    }

    // std::println("Rendered {} chunks", chunksRendered);

    // render water
    glDepthMask(GL_FALSE);

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        if(chunk->second->Visible) {
            chunk->second->RenderTranslucentMesh(m_Camera);
        }
    }

    glDepthMask(GL_TRUE);

    // render bounding box
    if(m_BlockHit.Hit) {
        m_BoundingBox.Render(m_Camera);
    }
}

Intersects::Frustum AppLayer::GetFrustum(const glm::mat4 viewProjectionMatrix) {
    Intersects::Frustum f;

    glm::vec4 rowX = glm::row(viewProjectionMatrix, 0);
    glm::vec4 rowY = glm::row(viewProjectionMatrix, 1);
    glm::vec4 rowZ = glm::row(viewProjectionMatrix, 2);
    glm::vec4 rowW = glm::row(viewProjectionMatrix, 3);

    glm::vec4 planes[6] = {
        rowW + rowX, // left
        rowW - rowX, // right
        rowW + rowY, // bottom
        rowW - rowY, // top
        rowW + rowZ, // near
        rowW - rowZ // far
    };

    for(int i = 0; i < 6; i++) {
        glm::vec3 normal = glm::vec3(planes[i]);
        float d = planes[i].w;

        float len = glm::length(normal);
        f.Faces[i].Normal = normal / len;
        f.Faces[i].D      = d / len;
    }

    return f;
}

void AppLayer::OnKeyEventHandler(const Core::Event& event) {
    std::println("Handling keyboard event: {}", event.Key);
}

void AppLayer::OnMouseButtonEventHandler(const Core::Event& event) {
    if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_LEFT) {
        if(m_BlockHit.Hit) {
            glm::vec3 cameraRay = m_Camera.CastRay();

            Intersects::FaceHit faceHit;
            glm::vec3 position = m_ChunkManager->GetChunk(m_BlockHit.Chunk)->GetBlockPosition(m_BlockHit.Block);
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
            
            bool isFaceHit = Intersects::RayFace(m_Camera.GetPosition(), cameraRay, modelMatrix, faceHit);

            if(isFaceHit) {
                glm::ivec3 newBlock = glm::ivec3(position) + glm::ivec3(faceHit.Normal);
                glm::ivec2 chunkPosition = m_ChunkManager->GetBlockChunk(newBlock);

                if(m_ChunkManager->GetBlockType(newBlock) == BlockType::AIR || m_ChunkManager->GetBlockType(newBlock) == BlockType::WATER) {
                    m_ChunkManager->SetBlockType(newBlock, BlockType::DIRT);

                    std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkPosition);
                    m_ChunkManager->BuildChunkMesh(chunk);
                }
            }
        }
    }

    if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_RIGHT) {
        if(m_BlockHit.Hit) {
            std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(m_BlockHit.Chunk);

            chunk->SetBlockType(m_BlockHit.Block, BlockType::AIR);

            // rebuild main chunk
            m_ChunkManager->BuildChunkMesh(chunk);

            // find neighbours chunks and rebuild those
            const glm::ivec2 chunkNeighbours[4] = {
                { -1,  0 },
                {  1,  0 },
                {  0, -1 },
                {  0,  1 }
            };

            for(const auto& neightbour : chunkNeighbours) {
                glm::ivec2 neightbourPosition = m_BlockHit.Chunk + neightbour;
                std::shared_ptr<Chunk> neightbourChunk = m_ChunkManager->GetChunk(neightbourPosition);

                m_ChunkManager->BuildChunkMesh(neightbourChunk);
            }
        }
    }
}

void AppLayer::UpdateChunks() {
    // generate chunks
    glm::ivec2 cameraChunk = AppLayer::WorldToChunkCoordinate(m_Camera.GetPosition());

    std::vector<glm::ivec2> chunksCreated;

    for(int x = -m_ViewDistance; x <= m_ViewDistance; x++) {
        for(int y = -m_ViewDistance; y <= m_ViewDistance; y++) {
            glm::ivec2 chunk = cameraChunk + glm::ivec2(x, y);
            float distance = glm::length(glm::vec2(chunk - cameraChunk));

            if(distance <= m_ViewDistance && !m_ChunkManager->ChunkExists(chunk)) {
                m_ChunkManager->CreateChunk(chunk);
                chunksCreated.push_back(chunk);
            }
        }
    }

    for(const auto& chunkPosition : chunksCreated) {
        std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkPosition);
        m_ChunkManager->BuildChunkMesh(chunk);
    }

    // remove chunks out of view distance
    std::vector<glm::ivec2> chunksToRemove;
    chunksToRemove.reserve(m_ViewDistance);

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        float distance = glm::length(glm::vec2(chunk->first - cameraChunk));

        if(distance > m_ViewDistance) {
            chunksToRemove.push_back(chunk->first);
        }
    }

    for(const auto& chunk : chunksToRemove) {
        m_ChunkManager->DestroyChunk(chunk);
    }
}

// TODO: move to chunk manager
glm::ivec2 AppLayer::WorldToChunkCoordinate(const glm::vec3& position) {
    return glm::ivec2(
        std::floor(position.x / Chunk::s_ChunkSize),
        std::floor(position.z / Chunk::s_ChunkSize)
    );
}