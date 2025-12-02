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
}

AppLayer::~AppLayer() {
}

void AppLayer::OnEvent(Core::Event& event) {
    Core::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<Core::KeyPressedEvent>([this](Core::KeyPressedEvent& e) { return OnKeyPressed(e); });
    dispatcher.Dispatch<Core::KeyReleasedEvent>([this](Core::KeyReleasedEvent& e) { return OnKeyRelease(e); });
    dispatcher.Dispatch<Core::MouseButtonPressedEvent>([this](Core::MouseButtonPressedEvent& e) { return OnMouseButtonPressed(e); });
    dispatcher.Dispatch<Core::MouseMovedEvent>([this](Core::MouseMovedEvent& e) { return OnMouseMoved(e); });
}

void AppLayer::OnUpdate(float deltaTime) {
    // update camera
    m_Camera.Update(deltaTime);
    
    // create/remove chunks out of view distance range
    UpdateChunks();

    // sort chunks from camera position
    SortChunks();

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
    // Core::Event positionUpdatedEvent = { 
    //     .Type = Core::EventType::PositionUpdated, 
    //     .Position = m_Camera.GetPosition()  
    // };

    // Core::Application::Get().GetEventDispatcher()->PushEvent(positionUpdatedEvent);
}

void AppLayer::OnRender() {
    // rendering commands
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_SkyBox.Render(m_Camera);

    RenderChunks();
    RenderBlockOutline();
}

bool AppLayer::OnKeyPressed(const Core::KeyPressedEvent& event) {
    // update camera controls
    if(m_Camera.ControlsActive.contains(event.GetKeyCode())) {
        m_Camera.ControlsActive[event.GetKeyCode()] = true;
    }

    return false;
}

bool AppLayer::OnKeyRelease(const Core::KeyReleasedEvent& event) {
    // update camera controls
    if(m_Camera.ControlsActive.contains(event.GetKeyCode())) {
        m_Camera.ControlsActive[event.GetKeyCode()] = false;
    }

    return false;
}

bool AppLayer::OnMouseButtonPressed(const Core::MouseButtonPressedEvent& event) {
    if(event.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT) {
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
                    chunk->LoadMesh();
                }
            }
        }
    }

    if(event.GetMouseButton() == GLFW_MOUSE_BUTTON_RIGHT) {
        if(m_BlockOutline.Visible) {
            Block removedBlock = m_ChunkManager->GetBlock(m_BlockOutline.Position);
            
            removedBlock.Type = BlockType::AIR;

            m_ChunkManager->CreateBlock(removedBlock);

            std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(removedBlock.Chunk);
            chunk->BuildMesh();
            chunk->LoadMesh();

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
                    neighborChunk->LoadMesh();
                }
            }
        }
    }

    return false;
}

bool AppLayer::OnMouseMoved(const Core::MouseMovedEvent& event) {
    // update camera
    float mouseSensitivity = 0.1f;
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();

    float yaw = m_Camera.GetYaw() - mouseSensitivity * (frameBufferSize.x / 2 - event.GetX());
    float pitch = m_Camera.GetPitch() + mouseSensitivity * (frameBufferSize.y / 2 - event.GetY());

    // clamp the pitch
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f ) pitch = -89.0f;

    m_Camera.SetYaw(yaw);
    m_Camera.SetPitch(pitch);

    return false;
}

void AppLayer::SortChunks() {
    // sort chunks based on distance from camera
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
}

void AppLayer::UpdateChunks() {
    // check timing
    float startTime = Core::Application::GetTime();
    float endTime = 0;

    // scan which chunks we need to remove/add based on view distance
    glm::ivec2 cameraChunk = WorldToChunkCoordinate(m_Camera.GetPosition());

    // remove chunks out of view distance
    std::vector<glm::ivec2> chunksRemoved;
    chunksRemoved.reserve(m_ViewDistance * m_ViewDistance * 4);

    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        float distance = glm::length(glm::vec2(chunk->first - cameraChunk));

        if(distance > m_ViewDistance + 1) {
            chunksRemoved.push_back(chunk->first);
        }
    }

    for(const auto& chunkKey : chunksRemoved) {
        m_ChunkManager->DestroyChunk(chunkKey);
    }

    // create chunks
    std::vector<std::shared_ptr<Chunk>> chunksCreated;
    chunksCreated.reserve(m_ViewDistance * m_ViewDistance * 4);

    const glm::ivec2 chunkNeighbors[4] = {
        {  0,  1 },
        {  1,  0 },
        {  0, -1 },
        { -1,  0 }
    };

    // in first pass, we make sure that all the necessary chunks are there
    for(int x = -m_ViewDistance; x <= m_ViewDistance; x++) {
        for(int y = -m_ViewDistance; y <= m_ViewDistance; y++) {
            glm::ivec2 chunkPosition = cameraChunk + glm::ivec2(x, y);
            float distance = glm::length(glm::vec2(chunkPosition - cameraChunk));

            if(distance <= m_ViewDistance) {
                if(m_ChunkManager->ChunkExists(chunkPosition)) {
                    std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkPosition);

                    if(chunk->GetState() == ChunkState::CREATED) {
                        chunk->SetState(ChunkState::DECORATED);

                        chunksCreated.push_back(chunk);
                    }
                } else {
                    std::shared_ptr<Chunk> chunk = m_ChunkManager->CreateChunk(chunkPosition);

                    chunk->Generate();
                    chunk->GenerateDecorations();
                    chunk->SetState(ChunkState::DECORATED);

                    chunksCreated.push_back(chunk);
                }

                // we do not need to mesh neighbors so we do not push it to the vector
                for(size_t i = 0; i < 4; i++) {
                    std::shared_ptr<Chunk> neighbor = m_ChunkManager->CreateChunk(chunkPosition + chunkNeighbors[i]);

                    if(neighbor && neighbor->GetState() == ChunkState::CREATED) {
                        neighbor->Generate();
                        neighbor->GenerateDecorations();
                    }
                }
            }
        }
    }

    endTime = Core::Application::GetTime();

    if(chunksCreated.size() > 0) {
        std::println("Generated {} chunks in {} seconds.", chunksCreated.size(), (endTime - startTime));
    }

    // // build meshes for the new chunks (excluding neighbors built on the fly)
    for(const auto& chunk : chunksCreated) {
        if(chunk->GetState() == ChunkState::DECORATED) {
            ChunkJob job;
            
            job.Type = ChunkJobType::MESH;
            job.Chunk = chunk;

            m_ChunkManager->AddChunkJob(job);

            chunk->SetState(ChunkState::MESHED);
        }
    }

    // load meshed chunks on GPU
    for(auto chunk = m_ChunkManager->ChunksBegin(); chunk != m_ChunkManager->ChunksEnd(); ++chunk) {
        if(chunk->second->GetState() == ChunkState::READY) {
            chunk->second->LoadMesh();
            chunk->second->SetState(ChunkState::LOADED);
        }
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

    for(const auto& chunkDistance : m_ChunksSorted) {
        std::shared_ptr<Chunk> chunk = m_ChunkManager->GetChunk(chunkDistance.Chunk);

        if(!chunk->Visible) {
            continue;
        }

        if(chunkDistance.Distance > viewDistanceSquared) {
            continue;
        }

        for(const auto& blockPosition : chunk->BlockVisible) {
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

                m_BlockOutline.Chunk = chunkDistance.Chunk;
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

        // Core::Event blockHitUpdatedEvent = { 
        //     .Type = Core::EventType::BlockHitUpdated, 
        //     .Position = m_BlockOutline.Position
        // };

        // Core::Application::Get().GetEventDispatcher()->PushEvent(blockHitUpdatedEvent);
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