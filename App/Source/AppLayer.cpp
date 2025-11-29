#include "AppLayer.h"

#include "Intersects.h"

#include "Core/Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <print>
#include <numeric>
#include <ranges>

AppLayer::AppLayer() {
    // create chunk manager
    m_ChunkManager = std::make_shared<ChunkManager>();

    // allocate memory for chunks sorting
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
    
    // create/remove chunks out of view distance range
    UpdateChunks();

    // sort chunks from camera position
    m_ChunksSorted.clear();

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        Intersects::AABB chunkBoundingBox = chunk->second->GetBoundingBox();
        
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

    // update chunk visibility
    Intersects::Frustum cameraFrustum = Intersects::GetFrustumFromViewProjectionMatrix(m_Camera.GetViewProjectionMatrix());

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        if(Intersects::AABBFrustum(cameraFrustum, chunk->second->GetBoundingBox())) {
            chunk->second->Visible = true;
        } else {
            chunk->second->Visible = false;
        }
    }

    // update outline block
    UpdateBlockOutline();

    // update sun 
    m_SkyBox.Update(deltaTime);

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

    m_SkyBox.Render(m_Camera);

    RenderChunks();
    RenderBlockOutline();
}

void AppLayer::OnKeyEventHandler(const Core::Event& event) {
    std::println("Handling keyboard event: {}", event.Key);
}

void AppLayer::OnMouseButtonEventHandler(const Core::Event& event) {
    if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_LEFT) {
        if(m_BlockOutline.Visible) {
            glm::vec3 cameraRay = m_Camera.CastRay();

            Intersects::FaceHit faceHit;
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_BlockOutline.Position);
            
            bool isFaceHit = Intersects::RayFace(m_Camera.GetPosition(), cameraRay, modelMatrix, faceHit);

            if(isFaceHit) {
                glm::vec3 newBlockPosition = m_BlockOutline.Position + faceHit.Normal;
                Block newBlock = m_ChunkManager->GetBlock(newBlockPosition);

                if(newBlock.Type == BlockType::AIR || newBlock.Type == BlockType::WATER) {
                    // add type selection;
                    newBlock.Type = BlockType::DIRT;
                    m_ChunkManager->CreateBlock(newBlock);

                    std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(newBlock.Chunk);
                    chunk->BuildMesh();
                }
            }
        }
    }

    if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_RIGHT) {
        if(m_BlockOutline.Visible) {
            Block removedBlock = m_ChunkManager->GetBlock(m_BlockOutline.Position);
            
            removedBlock.Type = BlockType::AIR;

            m_ChunkManager->CreateBlock(removedBlock);

            std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(removedBlock.Chunk);
            chunk->BuildMesh();

            // rebuild neighbors to avoid artifacts
            const glm::vec3 blockNeighbours[4] = {
                { -1.0f,  0.0f,  0.0f },
                {  1.0f,  0.0f,  0.0f },
                {  0.0f,  0.0f, -1.0f },
                {  0.0f,  0.0f,  1.0f }
            };

            for(const auto& neightbor : blockNeighbours) {
                glm::vec3 neightborPosition = m_BlockOutline.Position + neightbor;
                Block neighborBlock = m_ChunkManager->GetBlock(neightborPosition);

                if(neighborBlock.Chunk != removedBlock.Chunk) {
                    std::shared_ptr<Chunk> neighborChunk = m_ChunkManager->GetChunk(neighborBlock.Chunk);
                    neighborChunk->BuildMesh();
                }
            }
        }
    }
}

void AppLayer::UpdateChunks() {
    /*
        We can create a mutex chunk queue,
        which can be pulled from another thread to process it
    */

    // scan which chunks we need to remove/add based on view distance
    glm::ivec2 cameraChunk = WorldToChunkCoordinate(m_Camera.GetPosition());

    // remove chunks out of view distance
    std::vector<glm::ivec2> chunksRemoved;
    chunksRemoved.reserve(m_ViewDistance);

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        float distance = glm::length(glm::vec2(chunk->first - cameraChunk));

        if(distance > m_ViewDistance + 1) {
            chunksRemoved.push_back(chunk->first);
        }
    }

    for(const auto& chunk : chunksRemoved) {
        m_ChunkManager->DestroyChunk(chunk);
    }

    // generate chunks
    std::vector<glm::ivec2> chunksCreated;
    chunksCreated.reserve(m_ViewDistance);

    for(int x = -m_ViewDistance; x <= m_ViewDistance; x++) {
        for(int y = -m_ViewDistance; y <= m_ViewDistance; y++) {
            glm::ivec2 chunkPosition = cameraChunk + glm::ivec2(x, y);
            float distance = glm::length(glm::vec2(chunkPosition - cameraChunk));

            if(distance <= m_ViewDistance && !m_ChunkManager->ChunkExists(chunkPosition)) {
                chunksCreated.push_back(chunkPosition);
            }

            if(distance <= m_ViewDistance && m_ChunkManager->ChunkExists(chunkPosition)) {
                std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkPosition);

                if(chunk->GetState() == ChunkState::DECORATED) {
                    chunksCreated.push_back(chunkPosition);
                }
            }
        }
    }

    for(const auto& chunkKey : chunksCreated) {
        m_ChunkManager->CreateChunk(chunkKey);

        glm::ivec2 neighbors[4] = {
            {  0,  1 },
            {  1,  0 },
            {  0, -1 },
            { -1,  0 }
        };

        for(size_t i = 0; i < 4; i++) {
            m_ChunkManager->CreateChunk(chunkKey + neighbors[i]);
        }
    }

    // decorate new chunks
    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        if(chunk->second->GetState() == ChunkState::CREATED) {
            chunk->second->GenerateDecorations();
        }
    }

    // build meshes for the new chunks (excluding neighbors built on the fly)
    for(const auto& chunkKey : chunksCreated) {
        std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkKey);
        chunk->BuildMesh();
    }
}

void AppLayer::RenderChunks() {
    // render opaque mesh
    glEnable(GL_DEPTH_TEST);

    for(const auto& chunkKey : m_ChunksSorted) {
        std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkKey.Chunk);

        if(chunk->Visible) {
            chunk->RenderOpaqueMesh(m_Camera, m_SkyBox);
        }
    }

    // render translucent mesh
    glDepthMask(GL_FALSE);

    for(const auto& chunkKey : std::ranges::views::reverse(m_ChunksSorted)) {
        std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkKey.Chunk);

        if(chunk->Visible) {
            chunk->RenderTranslucentMesh(m_Camera, m_SkyBox);
        }
    }

    glDepthMask(GL_TRUE);
}

void AppLayer::UpdateBlockOutline() {
    glm::vec3 cameraRay = m_Camera.CastRay();

    float closestDistance = Chunk::s_ChunkSize; // maximum distance from which we pick up an object

    m_BlockOutline.Visible = false;

    const float viewDistanceSquared = Chunk::s_ChunkSize * Chunk::s_ChunkSize * 4;

    for(const auto& chunk : m_ChunksSorted) {
        if(!m_ChunkManager->GetChunk(chunk.Chunk)->Visible) {
            continue;
        }

        if(chunk.Distance > viewDistanceSquared) {
            continue;
        }

        for(const auto& blockPosition : m_ChunkManager->GetChunk(chunk.Chunk)->BlockVisible) {
            Block block = m_ChunkManager->GetBlock(blockPosition);

            if(block.Type == BlockType::AIR || block.Type == BlockType::WATER) {
                continue;
            }

            Intersects::AABB boundingBox = {
                block.Position - glm::vec3(0.5f),
                block.Position + glm::vec3(0.5f)
            };

            float distance;
            bool highlight = Intersects::RayAABB(m_Camera.GetPosition(), cameraRay, boundingBox, distance);

            if(highlight && distance < closestDistance) {
                closestDistance = distance;

                m_BlockOutline.Visible = true;

                m_BlockOutline.Chunk = chunk.Chunk;
                m_BlockOutline.Position = block.Position;
            }
        }

        if(m_BlockOutline.Visible) {
            break;
        }
    }

    if(m_BlockOutline.Visible) {
        Intersects::AABB box = {
            -glm::vec3(0.5f),
             glm::vec3(0.5f)
        };

        m_BlockOutline.BoundingBox.SetBoundingBox(box);
        m_BlockOutline.BoundingBox.SetPosition(m_BlockOutline.Position);
        m_BlockOutline.BoundingBox.Update();

        Core::Event blockHitUpdatedEvent = { 
            .Type = Core::EventType::BlockHitUpdated, 
            .Position = m_BlockOutline.Position
        };

        Core::Application::Get().GetEventDispatcher()->PushEvent(blockHitUpdatedEvent);
    }
}

void AppLayer::RenderBlockOutline() {
    if(m_BlockOutline.Visible) {
        m_BlockOutline.BoundingBox.Render(m_Camera);
    }
}

// TODO: move to chunk manager
glm::ivec2 AppLayer::WorldToChunkCoordinate(const glm::vec3& position) {
    return glm::ivec2(
        std::floor(position.x / Chunk::s_ChunkSize),
        std::floor(position.z / Chunk::s_ChunkSize)
    );
}