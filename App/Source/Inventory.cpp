#include "Inventory.h"

#include "Core/AppEvents.h"
#include "Core/Application.h"

#include <print>

Inventory::Inventory() {
    // fill the inventory with items
    m_Items.fill({ BlockType::VOID });

    m_Items[0] = { BlockType::DIRT };
    m_Items[1] = { BlockType::COBBLESTONE };
    m_Items[2] = { BlockType::SAND };
    m_Items[3] = { BlockType::WOOD };
    m_Items[4] = { BlockType::PLANKS };
    m_Items[5] = { BlockType::GLASS };

    InitBackground();
    InitSelectedItem();

    InitItems();
}

Inventory::~Inventory() {

}

void Inventory::Update() {
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    
    float singleOffset = 182 * 2 / static_cast<float>(m_Items.size());
    float position = frameBufferSize.x / 2 - 182 + (m_SelectedItem + 1) * singleOffset - singleOffset / 2;

    m_SelectedItemBackground.SetPosition({ position, 20.0f * 2, 0.0f });

    for(size_t i = 0; i < m_Items.size(); i++) {
        float itemPosition = frameBufferSize.x / 2 - 182 + (i + 1) * singleOffset - singleOffset / 2;

        m_Items[i].Quad.SetPosition({ itemPosition, 20.0f * 2, 0.0f });
    }

    Core::SelectedItemUpdatedEvent event(static_cast<int>(m_Items[m_SelectedItem].Type));
    Core::Application::Get().RaiseEvent(event);
}

void Inventory::Render(const glm::mat4& projection) {
    for(auto& item : m_Items) {
        if(item.Type == BlockType::VOID) {
            continue;
        }

        item.Quad.Render(projection);
    }

    m_SelectedItemBackground.Render(projection);
    m_Background.Render(projection);
}

void Inventory::SetSelectedItem(int offset) {
    if(m_SelectedItem == 0 && offset < 0) {
        m_SelectedItem = 0;
    } else if(m_SelectedItem + offset > m_Items.size() - 1) {
        return;
    } else {
        m_SelectedItem += offset;
    }
}

InventoryItem Inventory::GetSelectedItem() {
    return m_Items[m_SelectedItem];
}

void Inventory::InitBackground() {
    // create shader & texture
    uint32_t shader = Renderer::CreateGraphicsShader("Shaders/HUDVertex.glsl", "Shaders/HUDFragment.glsl");

    int width = 256;
    int height = 64;

    uint32_t texture = Renderer::LoadTexture("Textures/inventory.png", width, height);

    // load background
    m_Background = Renderer::Quad();

    m_Background.SetShader(shader);
    m_Background.SetTexture(texture);

    // 182 x 22
    glm::vec2 uvs[] = {
        { 0.0f   / width,   0.0f / height },
        { 182.0f / width,   0.0f / height },
        { 182.0f / width,  22.0f / height },
        { 0.0f   / width,  22.0f / height },
    };

    Renderer::Vertex vertices[] = {
        { { -0.5f, -0.5f, 0.0f }, uvs[0] },
        { {  0.5f, -0.5f, 0.0f }, uvs[1] },
        { {  0.5f,  0.5f, 0.0f }, uvs[2] },
        { { -0.5f,  0.5f, 0.0f }, uvs[3] }
    };

    m_Background.SetVertices(vertices);
    m_Background.InitGeometry();

    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    
    m_Background.SetScale({ 182.0f * 2, 22.0f * 2, 1.0f });
    m_Background.SetPosition({ frameBufferSize.x / 2, 20.0f * 2, 0.0f});
}

void Inventory::InitSelectedItem() {
    // create shader & texture
    uint32_t shader = Renderer::CreateGraphicsShader("Shaders/HUDVertex.glsl", "Shaders/HUDFragment.glsl");

    int width = 256;
    int height = 64;

    uint32_t texture = Renderer::LoadTexture("Textures/inventory.png", width, height);

    // load background
    m_SelectedItemBackground = Renderer::Quad();

    m_SelectedItemBackground.SetShader(shader);
    m_SelectedItemBackground.SetTexture(texture);

    // 24 x 24
    glm::vec2 uvs[] = {
        { 0.0f  / width,           22.0f / height },
        { 24.0f / width,           22.0f / height },
        { 24.0f / width, (22.0f + 24.0f) / height },
        { 0.0f  / width, (22.0f + 24.0f) / height }
    };

    Renderer::Vertex vertices[] = {
        { { -0.5f, -0.5f, 0.0f }, uvs[0] },
        { {  0.5f, -0.5f, 0.0f }, uvs[1] },
        { {  0.5f,  0.5f, 0.0f }, uvs[2] },
        { { -0.5f,  0.5f, 0.0f }, uvs[3] }
    };

    m_SelectedItemBackground.SetVertices(vertices);
    m_SelectedItemBackground.InitGeometry();

    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    
    m_SelectedItemBackground.SetScale({ 24.0f * 2, 24.0f * 2, 1.0f });
    m_SelectedItemBackground.SetPosition({ frameBufferSize.x / 2, 20.0f * 2, 0.0f});
}

void Inventory::InitItems() {
    std::unordered_map<BlockType, const std::filesystem::path> texturePaths = {
        { BlockType::DIRT, "Textures/Blocks/dirt.png" },
        { BlockType::COBBLESTONE, "Textures/Blocks/cobblestone.png" },
        { BlockType::WOOD, "Textures/Blocks/wood.png" },
        { BlockType::PLANKS, "Textures/Blocks/planks.png" },
        { BlockType::SAND, "Textures/Blocks/sand.png" },
        { BlockType::GLASS, "Textures/Blocks/glass.png" }
    };

    // create shader
    uint32_t shader = Renderer::CreateGraphicsShader("Shaders/HUDVertex.glsl", "Shaders/HUDFragment.glsl");

    for(auto& item : m_Items) {
        if(item.Type == BlockType::VOID) {
            continue;
        }

        int width = 300;
        int height = 300;

        uint32_t texture = Renderer::LoadTexture(texturePaths[item.Type], width, height);

        // load icon
        item.Quad.SetShader(shader);
        item.Quad.SetTexture(texture);
        
        item.Quad.InitGeometry();

        item.Quad.SetScale({ 12.0f * 2, 12.0f * 2, 1.0f });
    }
}