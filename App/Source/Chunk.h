#pragma once

#include "BlockRegistry.h"

#include "Core/Renderer/Shader.h"

#include <vector>
#include <memory>

class Chunk {
public:
    Chunk(int size, glm::ivec2 coordinate);
    ~Chunk();

    void Update(float deltaTime);
    void Render(std::shared_ptr<Renderer::Shader> shader, std::shared_ptr<BlockRegistry> blockRegistry);

    void Generate();

private:
    glm::ivec2 m_Coordinate;

    int m_Size = 16;
    int m_Height = 256;

    std::vector<Block> m_Blocks;
};