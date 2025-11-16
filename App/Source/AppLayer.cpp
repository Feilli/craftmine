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

    // init blocks
    // m_Blocks.reserve(m_ChunkSize * m_ChunkSize * m_ChunkSize);

    // // create an engine
    // std::default_random_engine engine(std::random_device{}());
    // std::uniform_int<> uniformInt(0, 2);

    // for(int i = 0; i < m_ChunkSize * m_ChunkSize; i++) {
    //     float x = i % m_ChunkSize;
    //     float z = std::floor(i / m_ChunkSize);
        
    //     m_Blocks.emplace_back();

    //     m_Blocks[i].m_Material = static_cast<BlockMaterial>(uniformInt(engine));
    //     m_Blocks[i].SetPosition(glm::vec3(x, 0.0f, z));
    // }

    // create chunks
    

    // load shaders
    m_Shader = std::make_shared<Renderer::Shader>("Shaders/Vertex.glsl", "Shaders/Fragment.glsl");

    // load texture
    m_TextureAtlas = std::make_shared<Renderer::TextureAtlas>("Textures/terrain.png", 16, 16);

    // create block registry and register blocks
    // we can easilly move it into config file :D
    
    // stone
    BlockType stone;
    stone.Material = BlockMaterial::STONE;

    stone.FaceToUVMap[BlockFace::FRONT] = m_TextureAtlas->GetTileUV(1, 0);
    stone.FaceToUVMap[BlockFace::BACK] = m_TextureAtlas->GetTileUV(1, 0);
    stone.FaceToUVMap[BlockFace::LEFT] = m_TextureAtlas->GetTileUV(1, 0);
    stone.FaceToUVMap[BlockFace::RIGHT] = m_TextureAtlas->GetTileUV(1, 0);
    stone.FaceToUVMap[BlockFace::TOP] = m_TextureAtlas->GetTileUV(1, 0);
    stone.FaceToUVMap[BlockFace::BOTTOM] = m_TextureAtlas->GetTileUV(1, 0);

    stone.Mesh = CreateBlockMesh(stone.FaceToUVMap);

    m_BlockRegistry.RegisterBlock(stone);

    // grass
    BlockType grass;
    grass.Material = BlockMaterial::GRASS;

    grass.FaceToUVMap[BlockFace::FRONT] = m_TextureAtlas->GetTileUV(3, 0);
    grass.FaceToUVMap[BlockFace::BACK] = m_TextureAtlas->GetTileUV(3, 0);
    grass.FaceToUVMap[BlockFace::LEFT] = m_TextureAtlas->GetTileUV(3, 0);
    grass.FaceToUVMap[BlockFace::RIGHT] = m_TextureAtlas->GetTileUV(3, 0);
    grass.FaceToUVMap[BlockFace::TOP] = m_TextureAtlas->GetTileUV(0, 0);
    grass.FaceToUVMap[BlockFace::BOTTOM] = m_TextureAtlas->GetTileUV(2, 0);

    grass.Mesh = CreateBlockMesh(grass.FaceToUVMap);

    m_BlockRegistry.RegisterBlock(grass);

    // dirt
    BlockType dirt;
    dirt.Material = BlockMaterial::DIRT;

    dirt.FaceToUVMap[BlockFace::FRONT] = m_TextureAtlas->GetTileUV(2, 0);
    dirt.FaceToUVMap[BlockFace::BACK] = m_TextureAtlas->GetTileUV(2, 0);
    dirt.FaceToUVMap[BlockFace::LEFT] = m_TextureAtlas->GetTileUV(2, 0);
    dirt.FaceToUVMap[BlockFace::RIGHT] = m_TextureAtlas->GetTileUV(2, 0);
    dirt.FaceToUVMap[BlockFace::TOP] = m_TextureAtlas->GetTileUV(2, 0);
    dirt.FaceToUVMap[BlockFace::BOTTOM] = m_TextureAtlas->GetTileUV(2, 0);

    dirt.Mesh = CreateBlockMesh(dirt.FaceToUVMap);

    m_BlockRegistry.RegisterBlock(dirt);

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
    // mark chunks for removal
    for(const auto& chunk: m_ChunkMap) {
        chunk.second->ToRemove = true;
    }

    // generate chunks
    glm::ivec2 cameraChunk = AppLayer::WorldToChunkCoordinate(m_Camera.GetPosition());

    for(int x = -m_ViewDistance; x <= m_ViewDistance; x++) {
        for(int y = -m_ViewDistance; y <= m_ViewDistance; y++) {
            glm::ivec2 chunkCoordinate = cameraChunk + glm::ivec2(x, y);

            if(!m_ChunkMap.contains(chunkCoordinate)) {
                m_ChunkMap[chunkCoordinate] = std::make_shared<Chunk>(m_ChunkSize, chunkCoordinate);
                m_ChunkMap[chunkCoordinate]->Generate();
            } else {
                m_ChunkMap[chunkCoordinate]->ToRemove = false;
            }
        }
    }

    // remove chunks marked for removal
    for(const auto& chunk : m_ChunkMap) {
        if(chunk.second->ToRemove) {
            m_ChunkMap.erase(chunk.first);
        }
    }

    // set chunk visibility
    Frustum cameraFrustum = GetFrustum(m_Camera.GetViewProjectionMatrix());

    for(const auto& chunk : m_ChunkMap) {
        AABB box = { 
            { chunk.first.x * m_ChunkSize, 0.0f, chunk.first.y * m_ChunkSize },
            { chunk.first.x * m_ChunkSize + m_ChunkSize, 256.0f, chunk.first.y * m_ChunkSize + m_ChunkSize }
        };

        if(AABBInFrustum(cameraFrustum, box)) {
            chunk.second->SetVisible(true);
        } else {
            chunk.second->SetVisible(false);
        }
    }

    // hightlist cube
    // float closestDistance = 16.0f; // maximum distance from which we pick up an object
    // int hightlighedCube = -1;

    // for(size_t i = 0; i < m_Blocks.size(); i++) {
    //     // reset highlight
    //     m_Blocks[i].m_Selected = false;

    //     // check if our awesome camera points on the block
    //     glm::vec3 minBound = m_Blocks[i].GetPosition() - glm::vec3(0.5f);
    //     glm::vec3 maxBound = m_Blocks[i].GetPosition() + glm::vec3(0.5f);

    //     float distance;
    //     bool highlight = RayIntersectsAABB(m_Camera.GetPosition(), m_CameraRay, minBound, maxBound, distance);

    //     if(highlight && distance < closestDistance) {
    //         closestDistance = distance;
    //         hightlighedCube = i;
    //     }
    // }

    // if(hightlighedCube >= 0) {
    //     m_Blocks[hightlighedCube].m_Selected = true;
    // }

    UpdateCameraRay();

    // update camera
    m_Camera.Update(deltaTime);
}

void AppLayer::OnRender() {
    // rendering commands
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render cube
    m_Shader->Use();

    // bind global uniforms
    m_Shader->SetMat4("u_Projection", m_Camera.GetProjectionMatrix());
    m_Shader->SetMat4("u_View", m_Camera.GetViewMatrix());

    // bind texture
    m_TextureAtlas->GetTexture()->Bind();
    
    for(auto chunk : m_ChunkMap) {
        chunk.second->Render(m_Shader, std::make_shared<BlockRegistry>(m_BlockRegistry));
    }

    // for(size_t i = 0; i < m_Blocks.size(); i++) {
    //     // bind uniforms
    //     m_Shader->SetMat4("u_Model", m_Blocks[i].GetModelMatrix());
    //     m_Shader->SetBool("u_Highlight", m_Blocks[i].m_Selected);

    //     auto mesh = m_BlockRegistry.Get(m_Blocks[i].m_Material).Mesh;
    //     mesh->Bind();

    //     glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    // }
}

AppLayer::Frustum AppLayer::GetFrustum(const glm::mat4 viewProjectionMatrix) {
    Frustum f;

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
        f.planes[i].normal = normal / len;
        f.planes[i].d      = d / len;
    }

    return f;
}

bool AppLayer::AABBInFrustum(const Frustum& frustum, const AABB& box) {
    for(int i = 0; i < 6; i++) {
        const Plane& p = frustum.planes[i];

        glm::vec3 positive;

        positive.x = (p.normal.x >= 0) ? box.maxBound.x : box.minBound.x;
        positive.y = (p.normal.y >= 0) ? box.maxBound.y : box.minBound.y;
        positive.z = (p.normal.z >= 0) ? box.maxBound.z : box.minBound.z;

        float distance = glm::dot(p.normal, positive) + p.d;

        if(distance < 0) {
            return false;
        }
    }

    return true;
}

void AppLayer::UpdateCameraRay() {
     // cast ray from the camera center
    glm::vec2 cursorPos = Core::Application::Get().GetCursorPos();
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();

    // normalize
    float x = (2.0f * cursorPos.x) / frameBufferSize.x - 1.0f;
    float y = 1.0f - (2.0f * cursorPos.y) / frameBufferSize.y;
    
    glm::vec2 normalizedCursorPos = glm::vec2(x, y);

    // convert normalize pos to world space
    glm::vec4 ray = glm::vec4(normalizedCursorPos.x, normalizedCursorPos.y, -1.0f, 1.0f);

    glm::vec4 rayProjection = glm::inverse(m_Camera.GetProjectionMatrix()) * ray;
    rayProjection.z = -1.0f;
    rayProjection.w = 0.0f;

    m_CameraRay = glm::normalize(glm::vec3(glm::inverse(m_Camera.GetViewMatrix()) * rayProjection));
}

bool AppLayer::RayIntersectsAABB(const glm::vec3& rayOrigin, 
                                 const glm::vec3& rayDirection, 
                                 const glm::vec3& minBound, 
                                 const glm::vec3& maxBound,
                                 float& tNear) {
    // check for x axis interception
    // t is the distance along the ray
    float txMin = (minBound.x - rayOrigin.x) / rayDirection.x;
    float txMax = (maxBound.x - rayOrigin.x) / rayDirection.x;
    
    if (txMin > txMax) {
        std::swap(txMin, txMax);
    }

    // check for y axis interception
    float tyMin = (minBound.y - rayOrigin.y) / rayDirection.y;
    float tyMax = (maxBound.y - rayOrigin.y) / rayDirection.y;
    if (tyMin > tyMax) {
        std::swap(tyMin, tyMax);
    }

    if(txMin > tyMax || tyMin > txMax) {
        return false;
    }

    txMin = glm::max(txMin, tyMin);
    txMax = glm::min(txMax, tyMax);

    // check for z axis interception
    float tzMin = (minBound.z - rayOrigin.z) / rayDirection.z;
    float tzMax = (maxBound.z - rayOrigin.z) / rayDirection.z;

    if(tzMin > tzMax) {
        std::swap(tzMin, tzMax);
    }

    if(txMin > tzMax || tzMin > txMax) {
        return false;
    }

    tNear = glm::max(txMin, tzMin);
    
    return tNear >= 0.0f;
}

bool AppLayer::RayIntersectsFace(const glm::vec3& rayOrigin, 
                                 const glm::vec3& rayDirection, 
                                 const glm::mat4& model, 
                                 FaceHit& hit) {
    // trasnform ray into cube local space
    glm::mat4 inverseModel = glm::inverse(model);
    glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(rayOrigin, 1.0f));
    glm::vec3 localDirection = glm::normalize(glm::vec3(inverseModel * glm::vec4(rayDirection, 0.0f)));

    float tMin = 10.f;
    int hitFace = -1;
    glm::vec3 hitNormal(0.0f);

    struct Plane { 
        glm::vec3 n;
        float d;
        int id;
    };

    Plane planes[] = {
        {{ 1.0f, 0.0f, 0.0f }, 0.5f, 0}, {{ -1.0f,  0.0f,  0.0f }, 0.5f, 1},
        {{ 0.0f, 1.0f, 0.0f }, 0.5f, 2}, {{  0.0f, -1.0f,  0.0f }, 0.5f, 3},
        {{ 0.0f, 1.0f, 1.0f }, 0.5f, 4}, {{  0.0f,  0.0f, -1.0f }, 0.5f, 5}
    };

    for(const Plane& p : planes) {
        float denom = glm::dot(p.n, localDirection);

        if(fabs(denom) < 1e-6f) {
            continue;
        }

        float t = (p.d - glm::dot(p.n, localOrigin)) / denom;

        if(t < 0.0f) {
            continue;
        }

        glm::vec3 hit = localOrigin + t * localDirection;

        // check bound on other two axes
        if(fabs(hit.x) <= 0.5f + 1e-6f &&
            fabs(hit.y) <= 0.5f + 1e-6f &&
            fabs(hit.z) <= 0.5f + 1e-6f)  {
            // 
            if(p.id == 0 || p.id == 1) {
                if(fabs(hit.y) > 0.5f || fabs(hit.z) > 0.5f) {
                    continue;
                }
            }

            if(p.id == 2 || p.id == 3) {
                if(fabs(hit.x) > 0.5f || fabs(hit.z) > 0.5f) {
                    continue;
                }
            }
            
            if(p.id == 4 || p.id == 5) {
                if(fabs(hit.x) > 0.5f || fabs(hit.y) > 0.5f) {
                    continue;
                }
            }

            if(t < tMin) {
                tMin = t;
                hitFace = p.id;
                hitNormal = p.n;
            }
        }
    }

    if(hitFace >= 0) {
        hit.T = tMin;
        hit.Face = hitFace;
        hit.Normal = glm::mat3(model) * hitNormal;
        return true;
    }

    return false;
}

void AppLayer::OnKeyEventHandler(const Core::Event& event) {
    std::println("Handling keyboard event: {}", event.Key);
}

int AppLayer::GetSelectedBlockIndex() {
    // for(size_t i = 0; i < m_Blocks.size(); i++) {
    //     if(m_Blocks[i].m_Selected) {
    //         return (int)i;
    //     }
    // }

    return -1;
}

void AppLayer::OnMouseButtonEventHandler(const Core::Event& event) {
    // if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_LEFT) {
    //     int selectedBlockIndex = GetSelectedBlockIndex();

    //     if(selectedBlockIndex >= 0) {
    //         FaceHit faceHit;
    //         bool isFaceHit = RayIntersectsFace(m_Camera.GetPosition(), m_CameraRay, m_Blocks[selectedBlockIndex].GetModelMatrix(), faceHit);

    //         if(isFaceHit) {
    //             Block newBlock;
    //             newBlock.SetPosition(m_Blocks[selectedBlockIndex].GetPosition() + faceHit.Normal);

    //             m_Blocks.push_back(newBlock);
    //         }
    //     }
    // }

    // if(event.Type == Core::EventType::MouseButtonReleased && event.Key == GLFW_MOUSE_BUTTON_RIGHT) {
    //     int selectedBlockIndex = GetSelectedBlockIndex();

    //     if(selectedBlockIndex >= 0) {
    //         m_Blocks.erase(m_Blocks.begin() + selectedBlockIndex);
    //     }
    // }
}

std::shared_ptr<Renderer::Mesh> AppLayer::CreateBlockMesh(std::unordered_map<BlockFace, Renderer::UVRect>& uvMap) {
    const std::vector<glm::vec3> vertices = {
        // front face
        { -0.5f, -0.5f,  0.5f },
        {  0.5f, -0.5f,  0.5f },
        {  0.5f,  0.5f,  0.5f },
        { -0.5f,  0.5f,  0.5f },

        // back face
        {  0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        { -0.5f,  0.5f, -0.5f },
        {  0.5f,  0.5f, -0.5f },

        // left face
        { -0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f,  0.5f },
        { -0.5f,  0.5f,  0.5f },
        { -0.5f,  0.5f, -0.5f },
        
        // right face
        {  0.5f, -0.5f,  0.5f },
        {  0.5f, -0.5f, -0.5f },
        {  0.5f,  0.5f, -0.5f },
        {  0.5f,  0.5f,  0.5f },

        // top face
        { -0.5f,  0.5f,  0.5f },
        {  0.5f,  0.5f,  0.5f },
        {  0.5f,  0.5f, -0.5f },
        { -0.5f,  0.5f, -0.5f },

        // bottom face
        { -0.5f, -0.5f, -0.5f },
        {  0.5f, -0.5f, -0.5f },
        {  0.5f, -0.5f,  0.5f },
        { -0.5f, -0.5f,  0.5f }
    };

    const std::vector<unsigned int> indices = {
        // front face
         0,  1,  2,      2,  3,  0,
        // back face
         4,  5,  6,      6,  7,  4,
        // left face
         8,  9, 10,     10, 11,  8,
        // right face
        12, 13, 14,     14, 15, 12,
        // top face
        16, 17, 18,     18, 19, 16,
        // bottom
        20, 21, 22,     22, 23, 20
    };

    const std::vector<glm::vec2> uvs = {
        // front face
        { uvMap[BlockFace::FRONT].UMin, uvMap[BlockFace::FRONT].VMin },
        { uvMap[BlockFace::FRONT].UMax, uvMap[BlockFace::FRONT].VMin },
        { uvMap[BlockFace::FRONT].UMax, uvMap[BlockFace::FRONT].VMax },
        { uvMap[BlockFace::FRONT].UMin, uvMap[BlockFace::FRONT].VMax },

        // back face
        { uvMap[BlockFace::BACK].UMin, uvMap[BlockFace::BACK].VMin },
        { uvMap[BlockFace::BACK].UMax, uvMap[BlockFace::BACK].VMin },
        { uvMap[BlockFace::BACK].UMax, uvMap[BlockFace::BACK].VMax },
        { uvMap[BlockFace::BACK].UMin, uvMap[BlockFace::BACK].VMax },

        // left face
        { uvMap[BlockFace::LEFT].UMin, uvMap[BlockFace::LEFT].VMin },
        { uvMap[BlockFace::LEFT].UMax, uvMap[BlockFace::LEFT].VMin },
        { uvMap[BlockFace::LEFT].UMax, uvMap[BlockFace::LEFT].VMax },
        { uvMap[BlockFace::LEFT].UMin, uvMap[BlockFace::LEFT].VMax },
        
        // right face
        { uvMap[BlockFace::RIGHT].UMin, uvMap[BlockFace::RIGHT].VMin },
        { uvMap[BlockFace::RIGHT].UMax, uvMap[BlockFace::RIGHT].VMin },
        { uvMap[BlockFace::RIGHT].UMax, uvMap[BlockFace::RIGHT].VMax },
        { uvMap[BlockFace::RIGHT].UMin, uvMap[BlockFace::RIGHT].VMax },

        // top face
        { uvMap[BlockFace::TOP].UMin, uvMap[BlockFace::TOP].VMin },
        { uvMap[BlockFace::TOP].UMax, uvMap[BlockFace::TOP].VMin },
        { uvMap[BlockFace::TOP].UMax, uvMap[BlockFace::TOP].VMax },
        { uvMap[BlockFace::TOP].UMin, uvMap[BlockFace::TOP].VMax },

        // bottom face
        { uvMap[BlockFace::BOTTOM].UMin, uvMap[BlockFace::BOTTOM].VMin },
        { uvMap[BlockFace::BOTTOM].UMax, uvMap[BlockFace::BOTTOM].VMin },
        { uvMap[BlockFace::BOTTOM].UMax, uvMap[BlockFace::BOTTOM].VMax },
        { uvMap[BlockFace::BOTTOM].UMin, uvMap[BlockFace::BOTTOM].VMax }
    };

    // load mesh
    return std::make_shared<Renderer::Mesh>(vertices, uvs, indices);
}

glm::ivec2 AppLayer::WorldToChunkCoordinate(const glm::vec3& position) {
    return glm::ivec2(
        std::floor(position.x / m_ChunkSize),
        std::floor(position.z / m_ChunkSize)
    );
}