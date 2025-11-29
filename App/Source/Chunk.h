#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/TextureAtlas.h"

#include "Camera.h"
#include "Perlin.h"
#include "SkyBox.h"
#include "Intersects.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

enum BlockType {
    VOID,
    AIR,
    STONE,
    DIRT,
    GRASS,
    WATER,
    SAND,
    WOOD,
    LEAVES
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

struct BlockMesh {
    BlockType Type = BlockType::AIR;

    bool Visible = false;

    std::vector<Renderer::Vertex> Vertices;
    std::vector<uint32_t> Indices;

    int IndexOffset = 0;
};

struct Face {
    Direction Direction;
    glm::vec3 Normal;
    glm::vec3 Vertices[4];
};

static const Face s_Faces[6] = {
    // font
    { 
        Direction::FRONT,
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
        Direction::BACK,
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
        Direction::LEFT,
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
        Direction::RIGHT,
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
        Direction::TOP,
        { 0.0f, -1.0f, 0.0f },
        {
            { -0.5f,  0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f },
            {  0.5f,  0.5f, -0.5f },
            { -0.5f,  0.5f, -0.5f },
        }
    },
    // bottom
    {
        Direction::BOTTOM,
        { 0.0f, 1.0f, 0.0f },
        {
            { -0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f,  0.5f },
            { -0.5f, -0.5f,  0.5f }
        }
    }
};

struct Decoration {
    glm::vec3 Position;
    BlockType Type;
};

static const Decoration s_Tree[22] = {
    { {  0,  5,  0 }, BlockType::LEAVES },

    { { -1,  4,  0 }, BlockType::LEAVES },
    { { -1,  4, -1 }, BlockType::LEAVES },
    { {  0,  4, -1 }, BlockType::LEAVES },
    { {  1,  4, -1 }, BlockType::LEAVES },
    { {  1,  4,  0 }, BlockType::LEAVES },
    { {  1,  4,  1 }, BlockType::LEAVES },
    { {  0,  4,  1 }, BlockType::LEAVES },
    { { -1,  4,  1 }, BlockType::LEAVES },
    { {  0,  4,  0 }, BlockType::LEAVES },

    { { -1,  3,  0 }, BlockType::LEAVES },
    { { -1,  3, -1 }, BlockType::LEAVES },
    { {  0,  3, -1 }, BlockType::LEAVES },
    { {  1,  3, -1 }, BlockType::LEAVES },
    { {  1,  3,  0 }, BlockType::LEAVES },
    { {  1,  3,  1 }, BlockType::LEAVES },
    { {  0,  3,  1 }, BlockType::LEAVES },
    { { -1,  3,  1 }, BlockType::LEAVES },
    { {  0,  3,  0 }, BlockType::WOOD },

    { {  0,  2,  0 }, BlockType::WOOD },
    { {  0,  1,  0 }, BlockType::WOOD },
    { {  0,  0,  0 }, BlockType::WOOD },
};

struct VertexNeighbors {
    glm::vec3 Neighbors[3];
};

enum ChunkState {
    CREATED,
    DECORATED,
    MESHED
};

class ChunkManager;

class Chunk {
public:
    Chunk(ChunkManager* chunkManager,
          glm::ivec2 key,
          const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas,
          const std::shared_ptr<Renderer::Shader>& shader);
    ~Chunk();

    void Generate();
    void GenerateDecorations();

    void ResetMesh();
    void BuildMesh();

    void RenderOpaqueMesh(const Camera& camera, const SkyBox& skybox);
    void RenderTranslucentMesh(const Camera& camera, const SkyBox& skybox);

    Intersects::AABB GetBoundingBox();

    glm::vec3 GetPosition();
    void SetPosition(const glm::vec3& position);

    BlockType GetBlockType(const glm::vec3& position);
    void SetBlockType(const glm::vec3 & position, const BlockType& type);

    ChunkState GetState();
public:
    bool Visible = false;
    std::vector<glm::vec3> BlockVisible;

    static const int s_ChunkSize = 16;
    static const int s_WaterLevel = 48;
private:
    void PlaceTree(const glm::vec3& position);

    bool FaceVisible(BlockType current, BlockType neighbor);

    uint8_t CreateVertexAO(const glm::vec3& position, const Direction& direction, const size_t& vertex);

    BlockMesh CreateBlockMesh(const glm::vec3& position, const BlockType& type);

    std::array<float, s_ChunkSize * s_ChunkSize> CreateHeightMap(const Perlin& perlin, 
                                                                 const float& scale = 0.01f, 
                                                                 const int& octaves = 4, 
                                                                 const float& persistence = 0.5f);
private:
    ChunkState m_State = ChunkState::CREATED;

    glm::ivec2 m_Key;
    glm::vec3 m_Position;

    ChunkManager* m_ChunkManager;

    Renderer::Mesh m_OpaqueMesh;
    Renderer::Mesh m_TranslucentMesh;

    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;
    std::shared_ptr<Renderer::Shader> m_Shader;

    std::unordered_map<BlockType, glm::ivec2> m_BlockTypesUVsMap[7];
    std::unordered_map<Direction, std::vector<glm::vec3>> m_VertexNeighbors[4];

    std::array<std::array<std::array<BlockType, s_ChunkSize>, s_ChunkSize * s_ChunkSize>, s_ChunkSize> m_BlockTypes{};
    Intersects::AABB m_BoundingBox;

    std::array<float, s_ChunkSize * s_ChunkSize> m_HeightMap = { 0.0f };
};
