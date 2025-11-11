#pragma once

#include "Texture.h"

#include <memory>
#include <filesystem>

namespace Renderer {
    
    struct UVRect {
        float UMin = 0.0f;
        float UMax = 1.0f;
        float VMin = 0.0f;
        float VMax = 1.0f;
    };

    class TextureAtlas {
    public:
        TextureAtlas(const std::filesystem::path& path, int width, int height);
        ~TextureAtlas();

        UVRect GetTileUV(int x, int y);
        std::shared_ptr<Texture> GetTexture();

    private:
        int m_Width = 0;
        int m_Height = 0;

        std::shared_ptr<Texture> m_Texture;
    };

}