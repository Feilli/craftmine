#pragma once

#include "Chunk.h"
#include "Camera.h"

#include "Core/Renderer/Quad.h"

#include <array>

struct InventoryItem {
    BlockType Type;
    Renderer::Quad Quad;
};

class Inventory {
public:
    Inventory();
    ~Inventory();

    void Update();
    void Render(const glm::mat4& projection);
    void RenderBackground(const glm::mat4& projection);
    void RenderSelectedItem(const glm::mat4& projection);
    void RenderItem(const InventoryItem& item, const int& index);

    void SetSelectedItem(int offset);
    InventoryItem GetSelectedItem();
private:
    void InitBackground();
    void InitSelectedItem();
    void InitItems();
private:
    static const int s_Size = 9;

    size_t m_SelectedItem = 0;
    std::array<InventoryItem, s_Size> m_Items;

    Renderer::Quad m_Background;
    Renderer::Quad m_SelectedItemBackground;
};