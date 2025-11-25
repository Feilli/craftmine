#include "ChunkManager.h"

#include <print>

ChunkManager::ChunkManager(const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas,
                           const std::shared_ptr<Renderer::Shader>& chunkShader,
                           const std::shared_ptr<Perlin>& perlinNoise) :
    m_TextureAtlas(textureAtlas),
    m_ChunkShader(chunkShader),
    m_PerlinNoise(perlinNoise) {
    
}

ChunkManager::~ChunkManager() {
    
}

std::shared_ptr<Chunk> ChunkManager::GetChunk(glm::ivec2 position) {
    if(ChunkExists(position)) {
        return m_Chunks[position];
    }

    return nullptr;
}

void ChunkManager::CreateChunk(glm::ivec2 position) {
    m_Chunks[position] = std::make_shared<Chunk>(m_TextureAtlas, m_ChunkShader);

    m_Chunks[position]->SetPosition({ position.x * Chunk::s_ChunkSize, 0, position.y * Chunk::s_ChunkSize });
    m_Chunks[position]->Generate(m_PerlinNoise);
}

void ChunkManager::BuildChunkMesh(std::shared_ptr<Chunk>& chunk) {
    BuildChunkOpaueMesh(chunk);
    BuildChunkWaterMesh(chunk);
}

void ChunkManager::BuildChunkOpaueMesh(std::shared_ptr<Chunk>& chunk) {
    // reset mesh
    chunk->ResetMesh();
    chunk->BlockVisible.clear();

    // build mesh
    std::vector<Renderer::Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t indexOffset = 0;

    for(int x = 0; x < Chunk::s_ChunkSize; x++) {
        for(int y = 0; y < Chunk::s_ChunkSize * Chunk::s_ChunkSize; y++) {
            for(int z = 0; z < Chunk::s_ChunkSize; z++) {
                glm::ivec3 localPosition = { x, y, z };

                BlockType type = chunk->GetBlockType(localPosition);

                if(type == BlockType::AIR || type == BlockType::WATER)
                    continue;

                glm::vec3 worldPosition = chunk->GetBlockPosition(localPosition); // returns world position
                Block block = BuildBlockMesh(chunk, worldPosition, type);

                if(block.Visible) {
                    vertices.insert(vertices.end(), block.Vertices.begin(), block.Vertices.end());

                    for(size_t i = 0; i < block.Indices.size(); i ++) {
                        indices.push_back(indexOffset + block.Indices[i]);
                    }

                    indexOffset += block.IndexOffset;

                    chunk->BlockVisible.push_back(localPosition);
                }
            }
        }
    }

    chunk->BuildMesh(vertices, indices);
}

void ChunkManager::BuildChunkWaterMesh(std::shared_ptr<Chunk>& chunk) {
    // reset mesh
    chunk->ResetWaterMesh();

    // build mesh
    std::vector<Renderer::Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t indexOffset = 0;

    for(int x = 0; x < Chunk::s_ChunkSize; x++) {
        for(int y = 0; y < Chunk::s_ChunkSize * Chunk::s_ChunkSize; y++) {
            for(int z = 0; z < Chunk::s_ChunkSize; z++) {
                glm::ivec3 localPosition = { x, y, z };

                BlockType type = chunk->GetBlockType(localPosition);

                if(type != BlockType::WATER)
                    continue;

                glm::vec3 worldPosition = chunk->GetBlockPosition(localPosition); // returns world position
                Block block = BuildBlockMesh(chunk, worldPosition, type);

                if(block.Visible) {
                    vertices.insert(vertices.end(), block.Vertices.begin(), block.Vertices.end());

                    for(size_t i = 0; i < block.Indices.size(); i ++) {
                        indices.push_back(indexOffset + block.Indices[i]);
                    }

                    indexOffset += block.IndexOffset;

                    chunk->BlockVisible.push_back(localPosition);
                }
            }
        }
    }

    chunk->BuildWaterMesh(vertices, indices);
}

Block ChunkManager::BuildBlockMesh(std::shared_ptr<Chunk>& chunk, glm::ivec3 position, BlockType type) {
    Block block;

    block.Type = type;

    const glm::ivec3 directions[6] = {
        {  0,  0,  1 }, // front
        {  0,  0, -1 }, // back
        { -1,  0,  0 }, // left
        {  1,  0,  0 }, // right
        {  0,  1,  0 }, // top
        {  0, -1,  0 } // bottom
    };

    for(int face = 0; face < 6; face++) {
        glm::ivec3 n = directions[face];
        BlockType neighbor = GetBlockType(position + n);

        if(!FaceVisible(block.Type, neighbor)) {
            continue;
        }

        const Face& f = s_Faces[face];

        for(int i = 0; i < 4; i++) {
            Renderer::Vertex v;
            v.Position = glm::vec3(position) + f.Vertices[i];

            std::vector<glm::vec2> uvs = chunk->GetBlockFaceUV(static_cast<Direction>(face), block.Type);

            if(uvs.size() > 1) {
                v.UVs = uvs[i];
            } else {
                v.UVs = uvs[0];
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

bool ChunkManager::FaceVisible(BlockType current, BlockType neighbor) {
    if(current == BlockType::AIR) {
        return false;
    }

    if(current == BlockType::WATER) {
        return neighbor != BlockType::WATER;
    }

    return neighbor == BlockType::AIR || neighbor == BlockType::WATER;
}

glm::ivec2 ChunkManager::GetBlockChunk(glm::ivec3 position) {
    glm::vec3 f = glm::vec3(position) / float(Chunk::s_ChunkSize);
    glm::ivec3 chunkPosition = glm::ivec3(glm::floor(f));
    return{ chunkPosition.x, chunkPosition.z };
}

void ChunkManager::SetBlockType(glm::ivec3 position, BlockType type) {
    glm::ivec2 chunk = GetBlockChunk(position);

    glm::ivec3 chunkPosition = { chunk.x, 0, chunk.y };
    glm::ivec3 localPosition = position - chunkPosition * Chunk::s_ChunkSize;

    m_Chunks[chunk]->SetBlockType(localPosition, type);
}

BlockType ChunkManager::GetBlockType(glm::ivec3 position) {
    glm::vec3 f = glm::vec3(position) / float(Chunk::s_ChunkSize);

    glm::ivec3 chunkPosition = glm::ivec3(glm::floor(f));
    chunkPosition.y = 0;

    glm::ivec3 localPosition = position - chunkPosition * Chunk::s_ChunkSize;

    glm::ivec2 chunk = { chunkPosition.x, chunkPosition.z };

    if(ChunkExists(chunk)) {
        return m_Chunks[chunk]->GetBlockType(localPosition);
    }

    return BlockType::AIR;
}

void ChunkManager::DestroyChunk(glm::ivec2 position) {
    m_Chunks.erase(position);
}

bool ChunkManager::ChunkExists(glm::ivec2 position) const {
    return m_Chunks.contains(position);
}

ChunkMapIterator ChunkManager::ChunksBegin() {
    return m_Chunks.begin();
}

ChunkMapIterator ChunkManager::ChunksEnd() {
    return m_Chunks.end();
}