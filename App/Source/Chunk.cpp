#include "Chunk.h"

Chunk::Chunk(int size, glm::ivec2 coordinate) {
    m_Size = size;
    m_Coordinate = coordinate;
       
    m_Blocks.reserve(static_cast<size_t>(m_Size * m_Size * m_Height));
}

Chunk::~Chunk() {

}

void Chunk::Update(float deltaTime) {
    // currently we do not have dynamic content in our chunks, so skip it for now
}

void Chunk::Render(std::shared_ptr<Renderer::Shader> shader, std::shared_ptr<BlockRegistry> blockRegistry) {
    for(size_t i = 0; i < m_Blocks.size(); i++) {
        // bind uniforms
        shader->SetMat4("u_Model", m_Blocks[i].GetModelMatrix());
        shader->SetBool("u_Highlight", m_Blocks[i].m_Selected);

        auto mesh = blockRegistry->Get(m_Blocks[i].m_Material).Mesh;
        mesh->Bind();

        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

void Chunk::Generate() {
    for(int i = 0; i < m_Size * m_Size; i++) {
        float x = i % m_Size;
        float z = std::floor(i / m_Size);
        
        m_Blocks.emplace_back();
        m_Blocks[i].SetPosition(glm::vec3(x + m_Coordinate.x * m_Size, 0.0f, z + m_Coordinate.y * m_Size));
    }
}
