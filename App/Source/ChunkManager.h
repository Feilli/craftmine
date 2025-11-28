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

struct Block {
    BlockType Type = BlockType::VOID;
    glm::ivec2 Chunk;
    glm::vec3 ChunkPosition;
    glm::vec3 Position;
};

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void CreateChunk(glm::ivec2 position);
    void DestroyChunk(glm::ivec2 position);

    bool ChunkExists(glm::ivec2 position) const;

    std::shared_ptr<Chunk> GetChunk(glm::ivec2 position);

    Block GetBlock(glm::vec3 position);
    void CreateBlock(const Block& block);

    ChunkMapIterator ChunksBegin();
    ChunkMapIterator ChunksEnd();
private:
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;
    std::shared_ptr<Renderer::Shader> m_ChunkShader;

    ChunkMap m_Chunks;
};
