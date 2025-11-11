#include "Chunk.h"

Chunk::Chunk() {
    m_Blocks.reserve(m_Size * m_Size * m_Height);
}

Chunk::~Chunk() {

}

void Chunk::Update(float deltaTime) {
    // currently we do not have dynamic content in our chunks, so skip it for now
}

void Chunk::Render() {
    for(const std::unique_ptr<Block>& block : m_Blocks) {
        block->Render();
    }
}

void Chunk::Generate(glm::vec2 offset) {

}