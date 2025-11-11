#include "Block.h"

#include <glm/gtc/matrix_transform.hpp>

Block::Block()
: m_Position(0.0f, 0.0f, 0.0f) {
    
}

Block::~Block() {

}

void Block::Update(float deltaTime) {

}

void Block::Render() {

}

void Block::SetPosition(glm::vec3 position) {
    m_Position = position;
}

glm::vec3 Block::GetPosition() {
    return m_Position;
}

glm::mat4 Block::GetModelMatrix() const {
    return glm::translate(glm::mat4(1.0f), m_Position);
}
