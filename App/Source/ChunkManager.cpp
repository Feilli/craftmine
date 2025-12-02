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

std::shared_ptr<Chunk> ChunkManager::CreateChunk(glm::ivec2 position) {
    if(ChunkExists(position)) {
        return nullptr;
    }

    std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(this, position, m_TextureAtlas, m_ChunkShader);
    chunk->SetPosition({ position.x * Chunk::s_ChunkSize, 0, position.y * Chunk::s_ChunkSize });

    {
        std::lock_guard<std::mutex> lock(m_ChunksMutex);
        m_Chunks[position] = chunk;
    }

    return chunk;
}

void ChunkManager::DestroyChunk(glm::ivec2 position) {
    {
        std::lock_guard<std::mutex> lock(m_ChunksMutex);
        m_Chunks.erase(position);
    }
}

bool ChunkManager::ChunkExists(glm::ivec2 position) {
    bool exists = false;

    {
        std::lock_guard<std::mutex> lock(m_ChunksMutex);
        exists = m_Chunks.contains(position);
    }

    return exists;
}

std::shared_ptr<Chunk> ChunkManager::GetChunk(glm::ivec2 position) {
    std::shared_ptr<Chunk> chunk = nullptr;

    if(ChunkExists(position)) {
        {
            std::lock_guard<std::mutex> lock(m_ChunksMutex);
            chunk = m_Chunks[position];
        }
    }

    return chunk;
}

Block ChunkManager::GetBlock(glm::vec3 position) {
    Block block;

    glm::vec3 chunkPosition = glm::floor(glm::vec3(position) / float(Chunk::s_ChunkSize));
    chunkPosition.y = 0;

    glm::vec3 localPosition = position - chunkPosition * float(Chunk::s_ChunkSize);

    glm::ivec2 chunk = { chunkPosition.x, chunkPosition.z };

    if(ChunkExists(chunk)) {
        block.Type = GetChunk(chunk)->GetBlockType(localPosition);
    }

    block.Position = position;
    block.ChunkPosition = localPosition;
    block.Chunk = chunk;

    return block;
}

void ChunkManager::CreateBlock(const Block& block) {
    if(ChunkExists(block.Chunk)) {
        GetChunk(block.Chunk)->SetBlockType(block.ChunkPosition, block.Type);
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
        
        switch(job.Type) {
            case ChunkJobType::GENERATE:
            {
                job.Chunk->Generate();
                break;
            }
            case ChunkJobType::DECORATE:
            {
                job.Chunk->GenerateDecorations();
                break;
            }
            case ChunkJobType::MESH:
            {
                job.Chunk->BuildMesh();
                job.Chunk->SetState(ChunkState::READY);
                break;
            }
        }
    }
}
