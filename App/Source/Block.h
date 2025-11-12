#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>

enum BlockMaterial {
    STONE,
    GRASS,
    DIRT
    // ...
};

class Block {
public:
    Block();
    ~Block();

    void Update(float deltaTime);
    void Render() const;

    void SetPosition(glm::vec3 position);

    BlockMaterial m_Material = BlockMaterial::STONE;

    glm::vec3 GetPosition();

    glm::mat4 GetModelMatrix() const;

    bool m_Selected = false;

private:
    glm::vec3 m_Position;
};
