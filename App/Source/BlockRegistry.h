#pragma once

#include "Block.h"

#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/TextureAtlas.h"

#include <stdint.h>
#include <memory>
#include <unordered_map>

enum BlockFace {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

struct BlockType {
    BlockMaterial Material = BlockMaterial::STONE;

    std::unordered_map<BlockFace, Renderer::UVRect> FaceToUVMap;
    std::shared_ptr<Renderer::Mesh> Mesh;

    bool Translucent = false;
};

class BlockRegistry {
public:
    BlockRegistry();
    ~BlockRegistry();

    void RegisterBlock(const BlockType& type);

    const BlockType& Get(BlockMaterial material);

private:
    std::unordered_map<BlockMaterial, BlockType> m_Blocks;
};

