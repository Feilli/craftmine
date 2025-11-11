#pragma once

#include "Block.h"

#include <vector>
#include <memory>

class Chunk {
public:
    Chunk();
    ~Chunk();

    void Update(float deltaTime);
    void Render();

    void Generate(glm::vec2 offset);

private:
    size_t m_Size = 16;
    size_t m_Height = 256;

    std::vector<std::unique_ptr<Block>> m_Blocks;
};