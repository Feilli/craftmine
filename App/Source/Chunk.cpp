#include "Chunk.h"
#include "ChunkManager.h"

#include <glm/gtc/matrix_transform.hpp>

#include <print>

Chunk::Chunk(ChunkManager* chunkManager,
             glm::ivec2 key,
             const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas,
             const std::shared_ptr<Renderer::Shader>& shader) {
    m_ChunkManager = chunkManager;
    m_Key = key;
    m_TextureAtlas = textureAtlas;
    m_Shader = shader;

    // fill the chunk with air (like a baloon) :D
    for(auto& a: m_BlockTypes) {
        for(auto& b : a) {
            b.fill(BlockType::AIR);
        }
    }

    // create block types
    // stone
    m_BlockTypesUVsMap[Direction::ALL][BlockType::STONE] = { 1, 0 };

    // grass
    m_BlockTypesUVsMap[Direction::FRONT][BlockType::GRASS] = { 3, 0 };
    m_BlockTypesUVsMap[Direction::BACK][BlockType::GRASS] = { 3, 0 };
    m_BlockTypesUVsMap[Direction::LEFT][BlockType::GRASS] = { 3, 0 };
    m_BlockTypesUVsMap[Direction::RIGHT][BlockType::GRASS] = { 3, 0 };
    m_BlockTypesUVsMap[Direction::TOP][BlockType::GRASS] = { 0, 0 };
    m_BlockTypesUVsMap[Direction::BOTTOM][BlockType::GRASS] = { 2, 0 };

    // dirt
    m_BlockTypesUVsMap[Direction::ALL][BlockType::DIRT] = { 2, 0 };

    // water
    m_BlockTypesUVsMap[Direction::ALL][BlockType::WATER] = { 14, 0 };

    // sand
    m_BlockTypesUVsMap[Direction::ALL][BlockType::SAND] = { 2, 1 };

    // wood
    m_BlockTypesUVsMap[Direction::FRONT][BlockType::WOOD] = { 4, 1 };
    m_BlockTypesUVsMap[Direction::BACK][BlockType::WOOD] = { 4, 1 };
    m_BlockTypesUVsMap[Direction::LEFT][BlockType::WOOD] ={ 4, 1 };
    m_BlockTypesUVsMap[Direction::RIGHT][BlockType::WOOD] = { 4, 1 };
    m_BlockTypesUVsMap[Direction::TOP][BlockType::WOOD] = { 5, 1 };
    m_BlockTypesUVsMap[Direction::BOTTOM][BlockType::WOOD] = { 5, 1 };

    // leaves
    m_BlockTypesUVsMap[Direction::ALL][BlockType::LEAVES] = { 6, 1 };

    // ambient occlusion
    // FRONT (+Z)
    m_VertexNeighbors[3][Direction::FRONT] = { { {  0, +1,  0 }, { -1,  0,  0 }, { -1, +1,  0 } } }; // v0
    m_VertexNeighbors[2][Direction::FRONT] = { { {  0, +1,  0 }, { +1,  0,  0 }, { +1, +1,  0 } } }; // v1
    m_VertexNeighbors[1][Direction::FRONT] = { { {  0, -1,  0 }, { +1,  0,  0 }, { +1, -1,  0 } } }; // v2
    m_VertexNeighbors[0][Direction::FRONT] = { { {  0, -1,  0 }, { -1,  0,  0 }, { -1, -1,  0 } } }; // v3

    // BACK (-Z)
    m_VertexNeighbors[3][Direction::BACK] = { { {  0, +1,  0 }, { +1,  0,  0 }, { +1, +1,  0 } } }; // v0
    m_VertexNeighbors[2][Direction::BACK] = { { {  0, +1,  0 }, { -1,  0,  0 }, { -1, +1,  0 } } }; // v1
    m_VertexNeighbors[1][Direction::BACK] = { { {  0, -1,  0 }, { -1,  0,  0 }, { -1, -1,  0 } } }; // v2
    m_VertexNeighbors[0][Direction::BACK] = { { {  0, -1,  0 }, { +1,  0,  0 }, { +1, -1,  0 } } }; // v3

    // LEFT (-X)
    m_VertexNeighbors[3][Direction::LEFT] = { { {  0, +1,  0 }, {  0,  0, -1 }, {  0, +1, -1 } } }; // v0
    m_VertexNeighbors[2][Direction::LEFT] = { { {  0, +1,  0 }, {  0,  0, +1 }, {  0, +1, +1 } } }; // v1
    m_VertexNeighbors[1][Direction::LEFT] = { { {  0, -1,  0 }, {  0,  0, +1 }, {  0, -1, +1 } } }; // v2
    m_VertexNeighbors[0][Direction::LEFT] = { { {  0, -1,  0 }, {  0,  0, -1 }, {  0, -1, -1 } } }; // v3

    // RIGHT (+X)
    m_VertexNeighbors[3][Direction::RIGHT] = { { {  0, +1,  0 }, {  0,  0, +1 }, {  0, +1, +1 } } }; // v0
    m_VertexNeighbors[2][Direction::RIGHT] = { { {  0, +1,  0 }, {  0,  0, -1 }, {  0, +1, -1 } } }; // v1
    m_VertexNeighbors[1][Direction::RIGHT] = { { {  0, -1,  0 }, {  0,  0, -1 }, {  0, -1, -1 } } }; // v2
    m_VertexNeighbors[0][Direction::RIGHT] = { { {  0, -1,  0 }, {  0,  0, +1 }, {  0, -1, +1 } } }; // v3

    // TOP (+Y)
    m_VertexNeighbors[3][Direction::TOP] = { { {  0,  0, -1 }, { -1,  0,  0 }, { -1,  0, -1 } } }; // v0
    m_VertexNeighbors[2][Direction::TOP] = { { {  0,  0, -1 }, { +1,  0,  0 }, { +1,  0, -1 } } }; // v1
    m_VertexNeighbors[1][Direction::TOP] = { { {  0,  0, +1 }, { +1,  0,  0 }, { +1,  0, +1 } } }; // v2
    m_VertexNeighbors[0][Direction::TOP] = { { {  0,  0, +1 }, { -1,  0,  0 }, { -1,  0, +1 } } }; // v3

    // BOTTOM (-Y)
    m_VertexNeighbors[0][Direction::BOTTOM] = { { {  0,  0, -1 }, { -1,  0,  0 }, { -1,  0, -1 } } }; // v0
    m_VertexNeighbors[1][Direction::BOTTOM] = { { {  0,  0, -1 }, { +1,  0,  0 }, { +1,  0, -1 } } }; // v1
    m_VertexNeighbors[2][Direction::BOTTOM] = { { {  0,  0, +1 }, { +1,  0,  0 }, { +1,  0, +1 } } }; // v2
    m_VertexNeighbors[3][Direction::BOTTOM] = { { {  0,  0, +1 }, { -1,  0,  0 }, { -1,  0, +1 } } }; // v3

    BlockVisible.reserve(s_ChunkSize * s_ChunkSize);
}

Chunk::~Chunk() {

}

void Chunk::Generate() {
    // create height map
    Perlin perlin(1234567890);
    m_HeightMap = CreateHeightMap(perlin, 0.01f, 4, 0.5f);

    // update block types
    for(int x = 0; x < s_ChunkSize; x++) {
        for(int z = 0; z < s_ChunkSize; z++) {
            // max height is 32 blocks
            int height = static_cast<int>(std::floor(m_HeightMap[z * s_ChunkSize + x] * s_ChunkSize * 3 + s_ChunkSize * 2));

            // fill chunk with stone
            for(int y = 0; y < height; y++) {
                m_BlockTypes[x][y][z] = BlockType::STONE;
            }

            // replace top chunks with grass and dirt
            m_BlockTypes[x][height - 1][z] = BlockType::GRASS;

            if(height > 3) {
                m_BlockTypes[x][height - 2][z] = BlockType::DIRT;
                m_BlockTypes[x][height - 3][z] = BlockType::DIRT;
                m_BlockTypes[x][height - 4][z] = BlockType::DIRT;
            }

            // everything that is height <= s_WaterLevel should be filled with water
            if(height < s_WaterLevel) {
                for(int y = height; y < s_WaterLevel; y++) {
                    m_BlockTypes[x][y][z] = BlockType::WATER;
                }

                m_BlockTypes[x][height - 1][z] = BlockType::SAND;
            }
        }
    }
}

void Chunk::GenerateDecorations() {
    // create heigh map
    Perlin perlin(1234567890);
    std::array<float, s_ChunkSize * s_ChunkSize> heightMap = CreateHeightMap(perlin, 0.6f, 4, 0.1f);

    // place trees
    float threshold = 0.75f;

    for(int x = 0; x < s_ChunkSize; x++) {
        for(int z = 0; z < s_ChunkSize; z++) {
            int height = static_cast<int>(std::floor(m_HeightMap[z * s_ChunkSize + x] * s_ChunkSize * 3 + s_ChunkSize * 2));

            BlockType type = m_BlockTypes[x][height][z];

            if(type == BlockType::WATER) {
                continue;
            }

            if(heightMap[z * s_ChunkSize + x] > threshold) {
                glm::vec3 position = { x, height, z };
                PlaceTree(position);
            }
        }
    }
}

void Chunk::ResetMesh() {
    m_OpaqueMesh.Reset();
    m_TranslucentMesh.Reset();
}

void Chunk::BuildMesh() {
    BlockVisible.clear();

    for(int x = 0; x < s_ChunkSize; x++) {
        for(int y = 0; y < s_ChunkSize * s_ChunkSize; y++) {
            for(int z = 0; z < s_ChunkSize; z++) {
                BlockType type = m_BlockTypes[x][y][z];

                if(type == BlockType::AIR)
                    continue;

                glm::vec3 position = glm::vec3(x, y, z) + m_Position;

                BlockMesh blockMesh = CreateBlockMesh(position, type);

                if(blockMesh.Visible) {
                    if(blockMesh.Type == BlockType::WATER || blockMesh.Type == BlockType::LEAVES) {
                        m_TranslucentMeshConfig.Vertices.insert(m_TranslucentMeshConfig.Vertices.end(), blockMesh.Vertices.begin(), blockMesh.Vertices.end());

                        for(size_t i = 0; i < blockMesh.Indices.size(); i ++) {
                            m_TranslucentMeshConfig.Indices.push_back(m_TranslucentMeshConfig.IndexOffset + blockMesh.Indices[i]);
                        }

                        m_TranslucentMeshConfig.IndexOffset += blockMesh.IndexOffset;
                    } else {
                        m_OpaqueMeshConfig.Vertices.insert(m_OpaqueMeshConfig.Vertices.end(), blockMesh.Vertices.begin(), blockMesh.Vertices.end());

                        for(size_t i = 0; i < blockMesh.Indices.size(); i ++) {
                            m_OpaqueMeshConfig.Indices.push_back(m_OpaqueMeshConfig.IndexOffset + blockMesh.Indices[i]);
                        }

                        m_OpaqueMeshConfig.IndexOffset += blockMesh.IndexOffset;
                    }
                    
                    BlockVisible.push_back(position);
                }
            }
        }
    }
}

void Chunk::LoadMesh() {
    ResetMesh();

    m_OpaqueMesh.Build(m_OpaqueMeshConfig.Vertices, m_OpaqueMeshConfig.Indices);
    m_TranslucentMesh.Build(m_TranslucentMeshConfig.Vertices, m_TranslucentMeshConfig.Indices);

    // clean up
    m_OpaqueMeshConfig.Vertices.clear();
    m_OpaqueMeshConfig.Indices.clear();
    m_OpaqueMeshConfig.IndexOffset = 0;

    m_TranslucentMeshConfig.Vertices.clear();
    m_TranslucentMeshConfig.Indices.clear();
    m_TranslucentMeshConfig.IndexOffset = 0;
}

void Chunk::RenderOpaqueMesh(const Camera& camera, const SkyBox& skybox) {
    if(m_OpaqueMesh.GetIndexCount() > 0) {
        // enable shader
        m_Shader->Use();

        // bind uniforms
        m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_Shader->SetMat4("u_View", camera.GetViewMatrix());
        m_Shader->SetMat4("u_Model", glm::mat4(1.0f));

        // bind lighting uniforms
        m_Shader->SetVec3("u_SunDirection", skybox.GetSunDirection());
        m_Shader->SetVec3("u_SunColor", skybox.GetSunColor());
        m_Shader->SetVec3("u_AmbientColor", skybox.GetAmbientColor());

        // distance fog
        m_Shader->SetVec3("u_CameraPosition", camera.GetPosition());
        m_Shader->SetFloat("u_FogStart", 160.0f);
        m_Shader->SetFloat("u_FogEnd", 176.0f);

        // bind texture atlas
        m_TextureAtlas->GetTexture()->Bind();

        // bind solid mesh
        m_OpaqueMesh.Bind();

        // draw chunk
        glDrawElements(GL_TRIANGLES, m_OpaqueMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

void Chunk::RenderTranslucentMesh(const Camera& camera, const SkyBox& skybox) {
    if(m_TranslucentMesh.GetIndexCount() > 0) {
        // enable shader
        m_Shader->Use();

        // bind uniforms
        m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_Shader->SetMat4("u_View", camera.GetViewMatrix());
        m_Shader->SetMat4("u_Model", glm::mat4(1.0f));

        // bind lighting uniforms
        m_Shader->SetVec3("u_SunDirection", skybox.GetSunDirection());
        m_Shader->SetVec3("u_SunColor", skybox.GetSunColor());
        m_Shader->SetVec3("u_AmbientColor", skybox.GetAmbientColor());

        // bind texture atlas
        m_TextureAtlas->GetTexture()->Bind();

        // bind water mesh
        m_TranslucentMesh.Bind();

        // draw water
        glDrawElements(GL_TRIANGLES, m_TranslucentMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

Intersects::AABB Chunk::GetBoundingBox() {
    return m_BoundingBox;
}

glm::vec3 Chunk::GetPosition() {
    return m_Position;
}

void Chunk::SetPosition(const glm::vec3& position) {
    // update position
    m_Position = position;

    // update bound box
    m_BoundingBox.MinBound = glm::vec3(m_Position);
    m_BoundingBox.MaxBound = { m_Position.x + Chunk::s_ChunkSize, m_Position.y + Chunk::s_ChunkSize * Chunk::s_ChunkSize, m_Position.z + Chunk::s_ChunkSize };
}

BlockType Chunk::GetBlockType(const glm::vec3& position) {
    if(position.x < 0 || position.x > m_BlockTypes.size() ||
        position.y < 0 || position.y > m_BlockTypes[0].size() ||
        position.z < 0 || position.z > m_BlockTypes[0][0].size()) {
        return BlockType::VOID;
    }

    glm::ivec3 pos = glm::ivec3(position);
    return m_BlockTypes[pos.x][pos.y][pos.z];
}

void Chunk::SetBlockType(const glm::vec3& position, const BlockType& type) {
    glm::ivec3 pos = glm::ivec3(position);
    m_BlockTypes[pos.x][pos.y][pos.z] = type;
}

void Chunk::SetState(const ChunkState& state) {
    m_State = state;
}

ChunkState Chunk::GetState() {
    return m_State;
}

void Chunk::PlaceTree(const glm::vec3& position) {
    for(const auto& treeBlock : s_Tree) {
        Block block = m_ChunkManager->GetBlock(m_Position + position + treeBlock.Position);
        
        block.Type = treeBlock.Type;

        m_ChunkManager->CreateBlock(block);
    }
}

bool Chunk::FaceVisible(BlockType current, BlockType neighbor) {
    if(current == BlockType::AIR) {
        return false;
    }

    if(current == BlockType::WATER) {
        return neighbor != BlockType::WATER && neighbor != BlockType::VOID;
    }

    return neighbor == BlockType::AIR || neighbor == BlockType::WATER || neighbor == BlockType::LEAVES;
}

uint8_t Chunk::CreateVertexAO(const glm::vec3& position, const Direction& direction, const size_t& vertex) {
    std::vector<glm::vec3> neighbors = m_VertexNeighbors[vertex][direction];
    std::array<bool, 3> solid = { false };

    for(size_t i = 0; i < 3; i++) {
        glm::vec3 neighborPosition = position + neighbors[i];
        Block block = m_ChunkManager->GetBlock(neighborPosition);

        if(block.Type != BlockType::AIR && block.Type != BlockType::WATER && block.Type != BlockType::LEAVES) {
            solid[i] = true;
        }
    }

    if(solid[0] && solid[1]) {
        return 0;
    }

    int occlusion = 0;

    if(solid[0]) occlusion++;
    if(solid[1]) occlusion++;
    if(solid[2]) occlusion++;

    return 3 - occlusion;
}

BlockMesh Chunk::CreateBlockMesh(const glm::vec3& position, const BlockType& type) {
    BlockMesh block;

    block.Type = type;

    const glm::vec3 directions[6] = {
        {  0,  0,  1 }, // front
        {  0,  0, -1 }, // back
        { -1,  0,  0 }, // left
        {  1,  0,  0 }, // right
        {  0,  1,  0 }, // top
        {  0, -1,  0 } // bottom
    };

    for(size_t face = 0; face < 6; face++) {
        glm::vec3 n = directions[face];
        Block neighbor = m_ChunkManager->GetBlock(position + n);

        if(!FaceVisible(block.Type, neighbor.Type)) {
            continue;
        }

        const Face& f = s_Faces[face];

        glm::ivec2 tile = { 0, 0 };

        if(m_BlockTypesUVsMap[Direction::ALL].contains(block.Type)) {
            tile = m_BlockTypesUVsMap[Direction::ALL][block.Type];
        } else {
            tile = m_BlockTypesUVsMap[f.Direction][block.Type];
        }

        std::vector<glm::vec2> uvs = m_TextureAtlas->GetTileUV(tile.x, tile.y);

        for(size_t i = 0; i < 4; i++) {
            Renderer::Vertex v;
            
            // position
            v.Position = position + f.Vertices[i];

            // uvs
            v.UVs = uvs[i];

            // noraml
            v.Normal = f.Normal;

            // ambient occlusion
            v.AO = CreateVertexAO(position + n, f.Direction, i);

            block.Vertices.push_back(v);
        }

        // Add indices
        block.Indices.push_back(block.IndexOffset + 0);
        block.Indices.push_back(block.IndexOffset + 1);
        block.Indices.push_back(block.IndexOffset + 2);
        block.Indices.push_back(block.IndexOffset + 2);
        block.Indices.push_back(block.IndexOffset + 3);
        block.Indices.push_back(block.IndexOffset + 0);

        block.IndexOffset += 4;

        block.Visible = true;
    }

    return block;
}

std::array<float, Chunk::s_ChunkSize * Chunk::s_ChunkSize> Chunk::CreateHeightMap(const Perlin& perlin,
                                                                                  const float& scale,
                                                                                  const int& octaves,
                                                                                  const float& persistence) {
    std::array<float, Chunk::s_ChunkSize * Chunk::s_ChunkSize> heightMap = { 0.0f };
    glm::ivec2 chunkOffset = { m_Position.x, m_Position.z }; // y - is height of a chunk

    for(int y = 0; y < s_ChunkSize; y++) {
        for(int x = 0; x < s_ChunkSize; x++) {
            double amplitude = 1.0;
            double freequency = 1.0;
            double noiseValue = 0.0;

            for(int o = 0; o < octaves; o++) {
                noiseValue += amplitude * perlin.Noise(
                    (chunkOffset.x + x) * scale * freequency,
                    (chunkOffset.y + y) * scale * freequency,
                    0.0
                );

                amplitude *= persistence;
                freequency *= 2.0;
            }

            // normalize to [0, 1]
            noiseValue = (noiseValue + 1.0) / 2.0;

            heightMap[y * s_ChunkSize + x] = (float)noiseValue;
        }
    }

    return heightMap;
}