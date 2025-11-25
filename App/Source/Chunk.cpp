#include "Chunk.h"

#include "Core/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <print>

Chunk::Chunk(const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas,
             const std::shared_ptr<Renderer::Shader>& shader) {
    m_Shader = shader;
    m_TextureAtlas = textureAtlas;

    // create block types
    // stone
    m_BlockTypesUVsMap[Direction::ALL][BlockType::STONE] = m_TextureAtlas->GetTileUV(1, 0);

    // grass
    m_BlockTypesUVsMap[Direction::FRONT][BlockType::GRASS] = m_TextureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::BACK][BlockType::GRASS] = m_TextureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::LEFT][BlockType::GRASS] = m_TextureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::RIGHT][BlockType::GRASS] = m_TextureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::TOP][BlockType::GRASS] = m_TextureAtlas->GetTileUV(0, 0);
    m_BlockTypesUVsMap[Direction::BOTTOM][BlockType::GRASS] = m_TextureAtlas->GetTileUV(2, 0);

    // dirt
    m_BlockTypesUVsMap[Direction::ALL][BlockType::DIRT] = m_TextureAtlas->GetTileUV(2, 0);

    // water
    m_BlockTypesUVsMap[Direction::ALL][BlockType::WATER] = m_TextureAtlas->GetTileUV(14, 0);

    // sand
    m_BlockTypesUVsMap[Direction::ALL][BlockType::SAND] = m_TextureAtlas->GetTileUV(2, 1);

    BlockVisible.reserve(s_ChunkSize * s_ChunkSize);
}

Chunk::~Chunk() {

}

void Chunk::Generate(const std::shared_ptr<Perlin>& perlin) {
    CreateHeightMap(perlin);

    for(int x = 0; x < s_ChunkSize; x++) {
        for(int z = 0; z < s_ChunkSize; z++) {
            // max height is 32 blocks
            int height = std::floor(m_HeightMap[z * s_ChunkSize + x] * s_ChunkSize * 3 + s_ChunkSize * 2);

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

            // everything that is height <= 64 should be filled with water
            if(height < s_OceanLevel) {
                for(int y = height; y < s_OceanLevel; y++) {
                    m_BlockTypes[x][y][z] = BlockType::WATER;
                }

                m_BlockTypes[x][height - 1][z] = BlockType::SAND;
            }
        }
    }
}

void Chunk::Update() {
    BuildSolidMesh();
    BuildWaterMesh();
}

void Chunk::ResetMesh() {
    m_Mesh.Reset();
}

void Chunk::BuildMesh(const std::vector<Renderer::Vertex>& vertices, const std::vector<uint32_t>& indices) {
    m_Mesh.Build(vertices, indices);
}

void Chunk::ResetWaterMesh() {
    m_WaterMesh.Reset();
}

void Chunk::BuildWaterMesh(const std::vector<Renderer::Vertex>& vertices, const std::vector<uint32_t>& indices) {
    m_WaterMesh.Build(vertices, indices);
}

void Chunk::BuildSolidMesh() {
    // build solid chunk mesh
    m_Mesh.Reset();

    std::vector<Renderer::Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t indexOffset = 0;

    BlockVisible.clear();

    for(int x = 0; x < s_ChunkSize; x++) {
        for(int y = 0; y < s_ChunkSize * s_ChunkSize; y++) {
            for(int z = 0; z < s_ChunkSize; z++) {
                BlockType type = m_BlockTypes[x][y][z];

                if(type == BlockType::AIR || type == BlockType::WATER)
                    continue;

                glm::vec3 blockPosition = glm::vec3(x, y, z);

                Block block = CreateBlock(blockPosition);

                if(block.Visible) {
                    vertices.insert(vertices.end(), block.Vertices.begin(), block.Vertices.end());

                    for(size_t i = 0; i < block.Indices.size(); i ++) {
                        indices.push_back(indexOffset + block.Indices[i]);
                    }

                    indexOffset += block.IndexOffset;

                    BlockVisible.push_back(blockPosition);
                }
            }
        }
    }

    m_Mesh.Build(vertices, indices);
}

void Chunk::BuildWaterMesh() {
    m_WaterMesh.Reset();

    std::vector<Renderer::Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t indexOffset = 0;

    for(int x = 0; x < s_ChunkSize; x++) {
        for(int y = 0; y < s_ChunkSize * s_ChunkSize; y++) {
            for(int z = 0; z < s_ChunkSize; z++) {
                BlockType type = m_BlockTypes[x][y][z];

                if(type != BlockType::WATER)
                    continue;

                glm::vec3 blockPosition = glm::vec3(x, y, z);

                Block block = CreateBlock(blockPosition);

                if(block.Visible) {
                    vertices.insert(vertices.end(), block.Vertices.begin(), block.Vertices.end());

                    for(size_t i = 0; i < block.Indices.size(); i ++) {
                        indices.push_back(indexOffset + block.Indices[i]);
                    }

                    indexOffset += block.IndexOffset;

                    BlockVisible.push_back(blockPosition);
                }
            }
        }
    }

    m_WaterMesh.Build(vertices, indices);
}

Block Chunk::CreateBlock(const glm::vec3& position) {
    Block block;

    block.Type = GetBlockType(position);

    const glm::vec3 directions[6] = {
        {  0,  0,  1 }, // front
        {  0,  0, -1 }, // back
        { -1,  0,  0 }, // left
        {  1,  0,  0 }, // right
        {  0,  1,  0 }, // top
        {  0, -1,  0 } // bottom
    };

    for(int face = 0; face < 6; face++) {
        glm::vec3 n = directions[face];
        BlockType neighbor = GetBlockType(position + n);

        if(!FaceVisible(block.Type, neighbor)) {
            continue;
        }

        const Face& f = s_Faces[face];

        for(int i = 0; i < 4; i++) {
            Renderer::Vertex v;
            v.Position = position + f.Vertices[i];

            if(m_BlockTypesUVsMap[Direction::ALL].contains(block.Type)) {
                v.UVs = m_BlockTypesUVsMap[Direction::ALL][block.Type][i];
            } else {
                v.UVs = m_BlockTypesUVsMap[face][block.Type][i];
            }

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

void Chunk::RenderOpaqueMesh(const Camera& camera) {
    // enable shader
    m_Shader->Use();

    // bind uniforms
    m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
    m_Shader->SetMat4("u_View", camera.GetViewMatrix());
    m_Shader->SetMat4("u_Model", glm::mat4(1.0f));

    // bind texture atlas
    m_TextureAtlas->GetTexture()->Bind();

    // bind solid mesh
    m_Mesh.Bind();

    // draw chunk
    glDrawElements(GL_TRIANGLES, m_Mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
}

void Chunk::RenderTranslucentMesh(const Camera& camera) {
    if(m_WaterMesh.GetIndexCount() > 0) {
        // enable shader
        m_Shader->Use();

        // bind uniforms
        m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_Shader->SetMat4("u_View", camera.GetViewMatrix());
        m_Shader->SetMat4("u_Model", glm::mat4(1.0f));

        // bind texture atlas
        m_TextureAtlas->GetTexture()->Bind();

        // bind water mesh
        m_WaterMesh.Bind();

        // draw water
        glDrawElements(GL_TRIANGLES, m_WaterMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

bool Chunk::FaceVisible(BlockType current, BlockType neighbor) {
    if(current == BlockType::AIR) {
        return false;
    }

    if(current == BlockType::WATER) {
        return neighbor != BlockType::WATER;
    }

    return neighbor == BlockType::AIR || neighbor == BlockType::WATER;
}

Intersects::AABB Chunk::GetBoundBox() const {
    return m_BoundBox;
}

void Chunk::SetPosition(glm::ivec3 position) {
    m_Position = position;

    // update bound box
    m_BoundBox.MinBound = glm::vec3(m_Position);
    m_BoundBox.MaxBound = { m_Position.x + Chunk::s_ChunkSize, m_Position.y + Chunk::s_ChunkSize * Chunk::s_ChunkSize, m_Position.z + Chunk::s_ChunkSize };
}

void Chunk::SetBlockType(glm::ivec3 position, BlockType type) {
    m_BlockTypes[position.x][position.y][position.z] = type;
}

BlockType Chunk::GetBlockType(glm::ivec3 position) const {
    return m_BlockTypes[position.x][position.y][position.z];
}

glm::vec3 Chunk::GetBlockPosition(glm::ivec3 position) {
    return m_Position + position;
}

bool Chunk::BlockInside(glm::ivec3 position) {
    return (position.x >= 0 && 
            position.y >= 0 && 
            position.z >= 0 &&
            position.x < s_ChunkSize &&
            position.y < s_ChunkSize * s_ChunkSize &&
            position.z < s_ChunkSize);
}

void Chunk::CreateHeightMap(const std::shared_ptr<Perlin>& perlin) {
    float scale = 0.01f;
    int octaves = 4;
    float persistence = 0.5f;

    glm::ivec2 chunkOffset = { m_Position.x, m_Position.z }; // y - is height of a chunk

    for(int y = 0; y < s_ChunkSize; y++) {
        for(int x = 0; x < s_ChunkSize; x++) {
            double amplitude = 1.0;
            double freequency = 1.0;
            double noiseValue = 0.0;

            for(int o = 0; o < octaves; o++) {
                noiseValue += amplitude * perlin->Noise(
                    (chunkOffset.x + x) * scale * freequency,
                    (chunkOffset.y + y) * scale * freequency,
                    0.0
                );

                amplitude *= persistence;
                freequency *= 2.0;
            }

            // normalize to [0, 1]
            noiseValue = (noiseValue + 1.0) / 2.0;

            m_HeightMap[y * s_ChunkSize + x] = (float)noiseValue;
        }
    }
}

std::vector<glm::vec2> Chunk::GetBlockFaceUV(Direction face, BlockType type) {
    if(m_BlockTypesUVsMap[Direction::ALL].contains(type)) {
        return m_BlockTypesUVsMap[Direction::ALL][type];
    }

    return m_BlockTypesUVsMap[face][type];
}