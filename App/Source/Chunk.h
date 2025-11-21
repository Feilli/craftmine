#pragma once

#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/TextureAtlas.h"

#include "Intersects.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

enum BlockType {
    AIR,
    STONE,
    DIRT,
    GRASS
};

enum Direction {
    FRONT = 0,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    ALL
};

struct Block {
    BlockType Type;

    bool Visible = false;

    std::vector<Renderer::Vertex> Vertices;
    std::vector<uint32_t> Indices;

    int IndexOffset = 0;
};

struct Face {
    glm::vec3 Normal;
    glm::vec3 Vertices[4];
};

static const Face s_Faces[6] = {
    // font
    { 
        { 0.0f, 0.0f, 1.0f },
        {
            { -0.5f, -0.5f,  0.5f },
            {  0.5f, -0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f },
            { -0.5f,  0.5f,  0.5f }
        }
    },
    // back
    {
        { 0.0f, 0.0f, -1.0f },
        {
            {  0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },
            { -0.5f,  0.5f, -0.5f },
            {  0.5f,  0.5f, -0.5f }
        }
    },
    // left
    {
        { -1.0f, 0.0f, 0.0f },
        {
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f,  0.5f },
            { -0.5f,  0.5f,  0.5f },
            { -0.5f,  0.5f, -0.5f }
        }
    },
    // right
    {
        { 1.0f, 0.0f, 0.0f },
        {
            {  0.5f, -0.5f,  0.5f },
            {  0.5f, -0.5f, -0.5f },
            {  0.5f,  0.5f, -0.5f },
            {  0.5f,  0.5f,  0.5f }
        }
    },
    // top
    {
        { 0.0f, 1.0f, 0.0f },
        {
            { -0.5f,  0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f },
            {  0.5f,  0.5f, -0.5f },
            { -0.5f,  0.5f, -0.5f },
        }
    },
    // bottom
    {
        { 0.0f, -1.0f, 0.0f },
        {
            { -0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f,  0.5f },
            { -0.5f, -0.5f,  0.5f }
        }
    }
};

class Chunk {

public:
    Chunk(const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas);
    ~Chunk();

    void Render();
    void Update();

    Block CreateBlock(const glm::vec3& position);

    bool FaceVisible(BlockType current, BlockType neighbor);

    bool ToRemove = false;
    bool Visible = false;

    void SetPosition(glm::ivec3 position);
    void SetBlockType(glm::ivec3 position, BlockType type);

    Intersects::AABB GetBoundBox() const;

    std::vector<glm::ivec3> BlockVisible;

    // TODO: we return block stuct querying by position
    BlockType GetBlockType(glm::ivec3 position);
    glm::vec3 GetBlockPosition(glm::ivec3 position);

    bool BlockInside(glm::ivec3 position);

    static const int s_ChunkSize = 16;

private:
    BlockType m_BlockTypes[s_ChunkSize][s_ChunkSize * s_ChunkSize][s_ChunkSize] = { BlockType::AIR };

    glm::ivec3 m_Position;
    Intersects::AABB m_BoundBox;

    std::unordered_map<BlockType, std::vector<glm::vec2>> m_BlockTypesUVsMap[7];
    
    Renderer::Mesh m_Mesh;
};