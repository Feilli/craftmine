#include "AppLayer.h"

#include "Perlin.h"

#include "Core/Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <print>
#include <numeric>

AppLayer::AppLayer() {
    // create a perlin noise
    // Perlin perlin;

    // const int width = 128;
    // const int height = 128;

    // std::vector<float> heightmap(width * height);

    // float scale = 0.005f;
    // int octaves = 4;
    // float persistence = 0.5f;

    // for(int y = 0; y < height; y++) {
    //     for(int x = 0; x < width; x++) {
    //         double amplitude = 1.0;
    //         double freequency = 1.0;
    //         double noiseValue = 0.0;

    //         for(int o = 0; o < octaves; o++) {
    //             noiseValue += amplitude * perlin.Noise(
    //                 x * scale * freequency,
    //                 y * scale * freequency,
    //                 0.0
    //             );

    //             amplitude *= persistence;
    //             freequency *= 2.0;
    //         }

    //         // normalize to [0, 1]
    //         noiseValue = (noiseValue + 1.0) / 2.0;

    //         heightmap[y * width + x] = (float)noiseValue;
    //     }
    // }

    // load shaders
    m_Shader = std::make_shared<Renderer::Shader>("Shaders/BlockVertex.glsl", "Shaders/BlockFragment.glsl");

    // load texture
    m_TextureAtlas = std::make_shared<Renderer::TextureAtlas>("Textures/terrain.png", 16, 16);
    
    Perlin m_PerlinNoise(1234567890);

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

    // mark chunks for removal
    std::vector<glm::ivec2> chunksToRemove;
    chunksToRemove.reserve(m_ViewDistance);

    for(const auto& chunk: m_ChunkMap) {
        chunk.second->ToRemove = true;
    }

    // generate chunks
    glm::ivec2 cameraChunk = AppLayer::WorldToChunkCoordinate(m_Camera.GetPosition());

    for(int x = -m_ViewDistance; x <= m_ViewDistance; x++) {
        for(int y = -m_ViewDistance; y <= m_ViewDistance; y++) {
            glm::ivec2 chunkCoordinate = cameraChunk + glm::ivec2(x, y);

            if(!m_ChunkMap.contains(chunkCoordinate)) {
                m_ChunkMap[chunkCoordinate] = std::make_shared<Chunk>(m_TextureAtlas);

                // generate chunk data (move position to constructor)
                m_ChunkMap[chunkCoordinate]->SetPosition({ chunkCoordinate.x * Chunk::s_ChunkSize, 0, chunkCoordinate.y * Chunk::s_ChunkSize });
                m_ChunkMap[chunkCoordinate]->Generate(m_PerlinNoise);

                // generate mesh
                m_ChunkMap[chunkCoordinate]->Update();
            } else {
                m_ChunkMap[chunkCoordinate]->ToRemove = false;
            }

            if(m_ChunkMap[chunkCoordinate]->ToRemove) {
                m_ChunkMap.erase(chunkCoordinate);
            }
        }
    }

    // set chunk visibility
    Intersects::Frustum cameraFrustum = GetFrustum(m_Camera.GetViewProjectionMatrix());

    for(const auto& chunk : m_ChunkMap) {
        if(Intersects::AABBFrustum(cameraFrustum, chunk.second->GetBoundBox())) {
            chunk.second->Visible = true;
        } else {
            chunk.second->Visible = false;
        }
    }

    // hightlight block
    float closestDistance = Chunk::s_ChunkSize; // maximum distance from which we pick up an object

    struct ChunkDistance { 
        glm::ivec2 Chunk;
        float Distance;
    };

    std::vector<ChunkDistance> chunksSorted;

    // find chunks to scan
    for(const auto& chunk : m_ChunkMap) {
        if(!chunk.second->Visible) {
            continue;
        }

        float distance;
        bool highlight = Intersects::RayAABB(m_Camera.GetPosition(), cameraRay, chunk.second->GetBoundBox(), distance);

        if(highlight && distance < closestDistance) {
            chunksSorted.push_back({ chunk.first, distance });
        }
    }

    std::sort(chunksSorted.begin(), chunksSorted.end(),
             [](auto& a, auto& b){ return a.Distance < b.Distance; });

    m_BlockHit.Hit = false;

    for(const auto& chunk : chunksSorted) {
        for(const auto& block : m_ChunkMap[chunk.Chunk]->BlockVisible) {
            BlockType type = m_ChunkMap[chunk.Chunk]->GetBlockType(block);

            if(type == BlockType::AIR) {
                continue;
            }

            glm::vec3 position = m_ChunkMap[chunk.Chunk]->GetBlockPosition(block);

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
    }

    if(m_BlockHit.Hit) {
        glm::vec3 position = m_ChunkMap[m_BlockHit.Chunk]->GetBlockPosition(m_BlockHit.Block);

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

    // render cube
    // TODO: move it inside of a chunk
    // TODO: Pass camera information to chunk
    m_Shader->Use();

    // bind global uniforms
    m_Shader->SetMat4("u_Projection", m_Camera.GetProjectionMatrix());
    m_Shader->SetMat4("u_View", m_Camera.GetViewMatrix());

    // bind texture atlas
    m_TextureAtlas->GetTexture()->Bind();
    
    for(auto chunk : m_ChunkMap) {
        if(chunk.second->Visible) {
            chunk.second->Render();
        }
    }

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

            glm::vec3 position = m_ChunkMap[m_BlockHit.Chunk]->GetBlockPosition(m_BlockHit.Block);
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

            bool isFaceHit = Intersects::RayFace(m_Camera.GetPosition(), cameraRay, modelMatrix, faceHit);

            if(isFaceHit) {
                glm::ivec3 newBlock = m_BlockHit.Block + glm::ivec3(faceHit.Normal);

                // check if block is out of chunk bounds
                if(m_ChunkMap[m_BlockHit.Chunk]->BlockInside(newBlock)) {
                    m_ChunkMap[m_BlockHit.Chunk]->SetBlockType(newBlock, BlockType::DIRT);
                    m_ChunkMap[m_BlockHit.Chunk]->Update();
                } else {
                    // find neighbour chunk
                    glm::ivec2 neighborChunk = m_BlockHit.Chunk + glm::ivec2(faceHit.Normal.x, faceHit.Normal.z);

                    if(newBlock.x < 0) {
                        newBlock.x = Chunk::s_ChunkSize + newBlock.x;
                    }

                    if(newBlock.y < 0) {
                        newBlock.y = Chunk::s_ChunkSize + newBlock.y;
                    }

                    if(newBlock.z < 0) {
                        newBlock.z = Chunk::s_ChunkSize + newBlock.z;
                    }

                    m_ChunkMap[neighborChunk]->SetBlockType(newBlock, BlockType::DIRT);
                    m_ChunkMap[neighborChunk]->Update();
                }
            }
        }
    }

    if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_RIGHT) {
        if(m_BlockHit.Hit) {
            m_ChunkMap[m_BlockHit.Chunk]->SetBlockType(m_BlockHit.Block, BlockType::AIR);
            m_ChunkMap[m_BlockHit.Chunk]->Update();
        }
    }
}

glm::ivec2 AppLayer::WorldToChunkCoordinate(const glm::vec3& position) {
    return glm::ivec2(
        std::floor(position.x / Chunk::s_ChunkSize),
        std::floor(position.z / Chunk::s_ChunkSize)
    );
}