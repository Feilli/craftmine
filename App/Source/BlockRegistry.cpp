#include "BlockRegistry.h"

BlockRegistry::BlockRegistry() {

}

BlockRegistry::~BlockRegistry() {

}

void BlockRegistry::RegisterBlock(const BlockType& type) {
        m_Blocks[type.Material] = type;
    }

const BlockType& BlockRegistry::Get(BlockMaterial material) {
    return m_Blocks[material];
}