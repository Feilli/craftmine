#pragma once

#include "Camera.h"
#include "Intersects.h"

#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Shader.h"

class BoundingBox {
public:
    BoundingBox();
    ~BoundingBox();

    void Update();
    void Render(const Camera& camera);

    void SetBoundingBox(Intersects::AABB box);
    void SetPosition(glm::vec3 position);

private:
    Intersects::AABB m_Box;
    glm::vec3 m_Position;

    Renderer::Shader m_Shader;
    Renderer::Mesh m_Mesh;
};