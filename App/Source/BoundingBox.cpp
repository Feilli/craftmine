#include "BoundingBox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>

BoundingBox::BoundingBox() :
    m_Shader("Shaders/BBVertex.glsl", "Shaders/BBFragment.glsl") {
}

BoundingBox::~BoundingBox() {

}

void BoundingBox::Update() {
    const std::vector<Renderer::Vertex> vertices = {
        { { m_Box.MinBound.x, m_Box.MinBound.y, m_Box.MinBound.z }, { 0, 0 } },
        { { m_Box.MaxBound.x, m_Box.MinBound.y, m_Box.MinBound.z }, { 0, 0 } }, 
        { { m_Box.MaxBound.x, m_Box.MinBound.y, m_Box.MaxBound.z }, { 0, 0 } },
        { { m_Box.MinBound.x, m_Box.MinBound.y, m_Box.MaxBound.z }, { 0, 0 } },
        { { m_Box.MinBound.x, m_Box.MaxBound.y, m_Box.MinBound.z }, { 0, 0 } },
        { { m_Box.MaxBound.x, m_Box.MaxBound.y, m_Box.MinBound.z }, { 0, 0 } }, 
        { { m_Box.MaxBound.x, m_Box.MaxBound.y, m_Box.MaxBound.z }, { 0, 0 } },
        { { m_Box.MinBound.x, m_Box.MaxBound.y, m_Box.MaxBound.z }, { 0, 0 } } 
    };

    const std::vector<uint32_t> indices = {
        // bottom rectangle
        0, 1,  1, 2,  2, 3,  3, 0,
        // top rectangle
        4, 5,  5, 6,  6, 7,  7, 4,
        // vertical pillars
        0, 4,  1, 5,  2, 6,  3, 7
    };

    m_Mesh.Reset();
    m_Mesh.Build(vertices, indices);
}

void BoundingBox::Render(const Camera& camera) {
    m_Shader.Use();

    m_Shader.SetMat4("u_Projection", camera.GetProjectionMatrix());
    m_Shader.SetMat4("u_View", camera.GetViewMatrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::scale(model, glm::vec3(1.005f));

    m_Shader.SetMat4("u_Model", model);

    m_Mesh.Bind();

    glDrawElements(GL_LINES, m_Mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
}

void BoundingBox::SetBoundingBox(Intersects::AABB box) {
    m_Box = box;
}

void BoundingBox::SetPosition(glm::vec3 position) {
    m_Position = position;
}