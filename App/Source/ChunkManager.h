#pragma once

#include "Perlin.h"
#include "Chunk.h"

#include "Core/Renderer/TextureAtlas.h"
#include "Core/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <map>
#include <queue>
#include <thread>
#include <memory>
#include <unordered_set>
#include <condition_variable>

struct ivec2_hash {
    std::size_t operator()(const glm::ivec2& v) const noexcept {
        // Convert to 64-bit to avoid overflow
        uint64_t x = static_cast<uint32_t>(v.x);
        uint64_t y = static_cast<uint32_t>(v.y);

        uint64_t hash = x * 73856093ull ^y * 73856093ull;
        return std::hash<uint64_t>()(hash);
    }
};

struct ivec2_eq {
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const noexcept {
        return a.x == b.x && a.y == b.y;
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

enum ChunkJobType {
    GENERATE,
    DECORATE,
    MESH
};

struct ChunkJob {
    ChunkJobType Type;
    std::shared_ptr<Chunk> Chunk;
};

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void AddChunkJob(const ChunkJob& job);

    std::shared_ptr<Chunk> CreateChunk(glm::ivec2 position);
    void DestroyChunk(glm::ivec2 position);

    bool ChunkExists(glm::ivec2 position);

    std::shared_ptr<Chunk> GetChunk(glm::ivec2 position);

    Block GetBlock(glm::vec3 position);
    void CreateBlock(const Block& block);

    ChunkMapIterator ChunksBegin();
    ChunkMapIterator ChunksEnd();
private:
    void ChunkJobsWorker();
private:
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;
    std::shared_ptr<Renderer::Shader> m_ChunkShader;

    ChunkMap m_Chunks;
    std::mutex m_ChunksMutex;
private:
    bool m_ChunkJobsWorkerRunning = true;
    
    std::thread m_ChunkJobsWorker;
    std::queue<ChunkJob> m_ChunkJobs;
    std::mutex m_ChunkJobsMutex;
    std::condition_variable m_ChunkJobsSignal;
};
