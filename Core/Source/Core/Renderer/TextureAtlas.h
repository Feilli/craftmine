#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

#include <memory>
#include <filesystem>

namespace Renderer {

    class TextureAtlas {
    public:
        TextureAtlas(const std::filesystem::path& path, int width, int height);
        ~TextureAtlas();

        std::vector<glm::vec2> GetTileUV(int x, int y) const;
        std::shared_ptr<Texture> GetTexture();

    private:
        int m_Width = 0;
        int m_Height = 0;

        std::shared_ptr<Texture> m_Texture;
    };

}