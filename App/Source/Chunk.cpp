#include "Chunk.h"

#include "Core/Renderer/Renderer.h"

#include <print>

Chunk::Chunk(const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas) {
    // Register block types

    // stone
    m_BlockTypesUVsMap[Direction::ALL][BlockType::STONE] = textureAtlas->GetTileUV(1, 0);

    // grass
    m_BlockTypesUVsMap[Direction::FRONT][BlockType::GRASS] = textureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::BACK][BlockType::GRASS] = textureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::LEFT][BlockType::GRASS] = textureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::RIGHT][BlockType::GRASS] = textureAtlas->GetTileUV(3, 0);
    m_BlockTypesUVsMap[Direction::TOP][BlockType::GRASS] = textureAtlas->GetTileUV(0, 0);
    m_BlockTypesUVsMap[Direction::BOTTOM][BlockType::GRASS] = textureAtlas->GetTileUV(2, 0);

    // dirt
    m_BlockTypesUVsMap[Direction::ALL][BlockType::DIRT] = textureAtlas->GetTileUV(2, 0);

    BlockVisible.reserve(s_ChunkSize * s_ChunkSize);
}

Chunk::~Chunk() {

}

void Chunk::Generate(const Perlin& perlin) {
    CreateHeightMap(perlin);
    
    for(int x = 0; x < s_ChunkSize; x++) {
        for(int z = 0; z < s_ChunkSize; z++) {
            // max height is 32 blocks
            int height = std::floor(m_HeightMap[z * s_ChunkSize + x] * s_ChunkSize * 2);

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
        }
    }
}

void Chunk::Update() {
    // build chunk mesh
    m_Mesh.Reset();

    std::vector<Renderer::Vertex> vertices;
    std::vector<uint32_t> indices;

    auto getBlock = [&](int x, int y, int z) -> BlockType {
        if(x < 0 || y < 0 || z < 0 ||
            x >= s_ChunkSize || y >= s_ChunkSize * s_ChunkSize || z >= s_ChunkSize)
            return BlockType::AIR;
        return m_BlockTypes[x][y][z];
    };

    BlockVisible.clear();

    uint32_t indexOffset = 0;

    for(int x = 0; x < s_ChunkSize; x++) {
        for(int y = 0; y < s_ChunkSize * s_ChunkSize; y++) {
            for(int z = 0; z < s_ChunkSize; z++) {
                BlockType type = m_BlockTypes[x][y][z];

                if(type == BlockType::AIR)
                    continue;

                // TODO: we can extract this code to change geometry of individual blocks;
                glm::vec3 blockPosition = glm::vec3(
                    m_Position.x + x,
                    m_Position.y + y,
                    m_Position.z + z
                );

                bool visible = false;

                for(int face = 0; face < 6; face++) {
                    const glm::ivec3 directions[6] = {
                        {  0,  0,  1 }, // front
                        {  0,  0, -1 }, // back
                        { -1,  0,  0 }, // left
                        {  1,  0,  0 }, // right
                        {  0,  1,  0 }, // top
                        {  0, -1,  0 } // bottom
                    };

                    glm::ivec3 n = directions[face];
                    BlockType neighbor = getBlock(x + n.x, y + n.y, z + n.z);

                    if(!FaceVisible(type, neighbor)) {
                        continue;
                    }

                    const Face& f = s_Faces[face];

                    for(int i = 0; i < 4; i++) {
                        Renderer::Vertex v;
                        v.Position = blockPosition + f.Vertices[i];

                        if(m_BlockTypesUVsMap[Direction::ALL].contains(type)) {
                            v.UVs = m_BlockTypesUVsMap[Direction::ALL][type][i];
                        } else {
                            v.UVs = m_BlockTypesUVsMap[face][type][i];
                        }

                        vertices.push_back(v);
                    }

                    // Add indices
                    indices.push_back(indexOffset + 0);
                    indices.push_back(indexOffset + 1);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 3);
                    indices.push_back(indexOffset + 0);

                    indexOffset += 4;

                    visible = true;
                }

                if(visible) {
                    BlockVisible.push_back({ x, y, z });
                }
            }
        }
    }

    m_Mesh.Build(vertices, indices);
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

void Chunk::Render() {
    m_Mesh.Bind();
    glDrawElements(GL_TRIANGLES, m_Mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
}

bool Chunk::FaceVisible(BlockType current, BlockType neighbor) {
    if(current == BlockType::AIR) {
        return false;
    }

    return neighbor == BlockType::AIR;
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

BlockType Chunk::GetBlockType(glm::ivec3 position) {
    if(position.x < 0 || position.y < 0 || position.z < 0 ||
        position.x >= s_ChunkSize || position.y >= s_ChunkSize * s_ChunkSize || position.z >= s_ChunkSize) {
        return BlockType::AIR;
    }

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

void Chunk::CreateHeightMap(const Perlin& perlin) {
    float scale = 0.005f;
    int octaves = 4;
    float persistence = 0.5f;

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

            m_HeightMap[y * s_ChunkSize + x] = (float)noiseValue;
        }
    }
}
