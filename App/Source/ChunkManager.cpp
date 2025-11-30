#include "ChunkManager.h"
#include "Chunk.h"

#include <print>

ChunkManager::ChunkManager() {
    m_TextureAtlas = std::make_shared<Renderer::TextureAtlas>("Textures/terrain.png", 16, 16);
    m_ChunkShader = std::make_shared<Renderer::Shader>("Shaders/ChunkVertex.glsl", "Shaders/ChunkFragment.glsl");

    m_ChunkJobsWorker = std::thread(&ChunkManager::ChunkJobsWorker, this);
}

ChunkManager::~ChunkManager() {
    m_ChunkJobsWorkerRunning = false;
    
    m_ChunkJobsSignal.notify_all();

    if(m_ChunkJobsWorker.joinable()) {
        m_ChunkJobsWorker.join();
    }
}

void ChunkManager::AddChunkJob(const ChunkJob& job) {
    {
        std::lock_guard<std::mutex> lock(m_ChunkJobsMutex);
        m_ChunkJobs.push(job);
    }

    m_ChunkJobsSignal.notify_one();
}

void ChunkManager::CreateChunk(glm::ivec2 position) {
    if(m_Chunks.contains(position)) {
        return;
    }

    m_Chunks[position] = std::make_shared<Chunk>(this, position, m_TextureAtlas, m_ChunkShader);

    m_Chunks[position]->SetPosition({ position.x * Chunk::s_ChunkSize, 0, position.y * Chunk::s_ChunkSize });
    // m_Chunks[position]->Generate();
}

void ChunkManager::DestroyChunk(glm::ivec2 position) {
    m_Chunks.erase(position);
}

bool ChunkManager::ChunkExists(glm::ivec2 position) const {
    return m_Chunks.contains(position);
}

std::shared_ptr<Chunk> ChunkManager::GetChunk(glm::ivec2 position) {
    if(ChunkExists(position)) {
        return m_Chunks[position];
    }

    return nullptr;
}

Block ChunkManager::GetBlock(glm::vec3 position) {
    Block block;

    glm::vec3 chunkPosition = glm::floor(glm::vec3(position) / float(Chunk::s_ChunkSize));
    chunkPosition.y = 0;

    glm::vec3 localPosition = position - chunkPosition * float(Chunk::s_ChunkSize);

    glm::ivec2 chunk = { chunkPosition.x, chunkPosition.z };

    if(ChunkExists(chunk)) {
        block.Type = m_Chunks[chunk]->GetBlockType(localPosition);
    }

    block.Position = position;
    block.ChunkPosition = localPosition;
    block.Chunk = chunk;

    return block;
}

void ChunkManager::CreateBlock(const Block& block) {
    if(ChunkExists(block.Chunk)) {
        std::shared_ptr<Chunk> chunk = m_Chunks[block.Chunk];
        chunk->SetBlockType(block.ChunkPosition, block.Type);
    }
}

ChunkMapIterator ChunkManager::ChunksBegin() {
    return m_Chunks.begin();
}

ChunkMapIterator ChunkManager::ChunksEnd() {
    return m_Chunks.end();
}

void ChunkManager::ChunkJobsWorker() {
    while(m_ChunkJobsWorkerRunning) {
        ChunkJob job;

        {
            std::unique_lock<std::mutex> lock(m_ChunkJobsMutex);
            m_ChunkJobsSignal.wait(lock, [this] { return !m_ChunkJobs.empty() || ! m_ChunkJobsWorkerRunning; });

            if(!m_ChunkJobsWorkerRunning) {
                break;
            }

            job = m_ChunkJobs.front();
            m_ChunkJobs.pop();
        }

        std::shared_ptr<Chunk> chunk = GetChunk(job.Chunk);

        switch(job.Type) {
            case ChunkJobType::GENERATE:
                chunk->Generate();
                break;
            case ChunkJobType::DECORATE:
                chunk->GenerateDecorations();
                break;
            case ChunkJobType::MESH:
                chunk->BuildMesh();
                chunk->SetState(ChunkState::READY);
                break;
        }
    }
}
