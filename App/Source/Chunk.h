#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/TextureAtlas.h"

#include "Camera.h"
#include "Intersects.h"
#include "Perlin.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

enum BlockType {
    AIR,
    STONE,
    DIRT,
    GRASS,
    WATER,
    SAND
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
    Chunk(const std::shared_ptr<Renderer::TextureAtlas>& textureAtlas,
          const std::shared_ptr<Renderer::Shader>& shader);
    ~Chunk();

    void Generate(const std::shared_ptr<Perlin>& perlin);
    void RenderOpaqueMesh(const Camera& camera);
    void RenderTranslucentMesh(const Camera& camera);

    void Update();

    void ResetMesh();
    void BuildMesh(const std::vector<Renderer::Vertex>& vertices, const std::vector<uint32_t>& indices);

    void ResetWaterMesh();
    void BuildWaterMesh(const std::vector<Renderer::Vertex>& vertices, const std::vector<uint32_t>& indices);

    void BuildSolidMesh();
    void BuildWaterMesh();

    Block CreateBlock(const glm::vec3& position);

    bool FaceVisible(BlockType current, BlockType neighbor);

    void SetPosition(glm::ivec3 position);
    void SetBlockType(glm::ivec3 position, BlockType type);

    Intersects::AABB GetBoundBox() const;

    // those should return and accept local position
    BlockType GetBlockType(glm::ivec3 position) const;
    glm::vec3 GetBlockPosition(glm::ivec3 position);

    bool BlockInside(glm::ivec3 position);

    void CreateHeightMap(const std::shared_ptr<Perlin>& perlin);

    std::vector<glm::vec2> GetBlockFaceUV(Direction face, BlockType type);
public:
    bool ToRemove = false;
    bool Visible = false;
    
    static const int s_ChunkSize = 16;
    static const int s_OceanLevel = 48;

    std::vector<glm::ivec3> BlockVisible;
private:
    ChunkManager* manager;

    BlockType m_BlockTypes[s_ChunkSize][s_ChunkSize * s_ChunkSize][s_ChunkSize] = { BlockType::AIR };

    glm::ivec3 m_Position;
    Intersects::AABB m_BoundBox;

    std::unordered_map<BlockType, std::vector<glm::vec2>> m_BlockTypesUVsMap[7];
    
    std::shared_ptr<Renderer::TextureAtlas> m_TextureAtlas;
    std::shared_ptr<Renderer::Shader> m_Shader;

    Renderer::Mesh m_Mesh;
    Renderer::Mesh m_WaterMesh;

    float m_HeightMap[s_ChunkSize * s_ChunkSize] = { 0.0f };
};