#pragma once

#include "Perlin.h"
#include "Chunk.h"

#include "Core/Renderer/TextureAtlas.h"
#include "Core/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <map>
#include <memory>

struct ivec2_hash {
    std::size_t operator()(const glm::ivec2& v) const noexcept {
        // Convert to 64-bit to avoid overflow
        uint64_t x = static_cast<uint32_t>(v.x);
        uint64_t y = static_cast<uint32_t>(v.y);

        uint64_t hash = x * 73856093ull ^y * 73856093ull;
        return std::hash<uint64_t>()(hash);
    }
};

using ChunkMap = std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>, ivec2_hash>;
using ChunkMapIterator = ChunkMap::const_iterator;

class Chunk;

class ChunkManager {
public:
    ChunkManager(const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas,
                 const std::shared_ptr<Renderer::Shader>& chunkShader,
                 const std::shared_ptr<Perlin>& perlinNoise);
    ~ChunkManager();

    void CreateChunk(glm::ivec2 position);
    void BuildChunkMesh(std::shared_ptr<Chunk>& chunk);
    void BuildChunkOpaueMesh(std::shared_ptr<Chunk>& chunk);
    void BuildChunkWaterMesh(std::shared_ptr<Chunk>& chunk);

    // in the entire chunks map
    BlockType GetBlockType(glm::ivec3 position);
    void SetBlockType(glm::ivec3 position, BlockType type);

    glm::ivec2 GetBlockChunk(glm::ivec3 position);
    
    Block BuildBlockMesh(std::shared_ptr<Chunk>& chunk, glm::ivec3 position, BlockType type);
    bool FaceVisible(BlockType current, BlockType neighbor);

    void DestroyChunk(glm::ivec2 position);

    bool ChunkExists(glm::ivec2 position) const;

    std::shared_ptr<Chunk> GetChunk(glm::ivec2 position);

    ChunkMapIterator ChunksBegin();
    ChunkMapIterator ChunksEnd();
private:
    // move on stack? why keep them shared ptrs?
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;
    std::shared_ptr<Renderer::Shader> m_ChunkShader;
    std::shared_ptr<Perlin> m_PerlinNoise;

    ChunkMap m_Chunks;
};