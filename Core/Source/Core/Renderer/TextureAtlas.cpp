#include "TextureAtlas.h"

namespace Renderer {

    TextureAtlas::TextureAtlas(const std::filesystem::path& path, int width, int height) {
        m_Texture = std::make_shared<Texture>(path);

        m_Width = width;
        m_Height = height;
    }

    TextureAtlas::~TextureAtlas() {

    }

    UVRect TextureAtlas::GetTileUV(int x, int y) {
        float tileW = 1.0f / m_Width;
        float tileH = 1.0f / m_Height;

        UVRect rect;
        rect.UMin = x * tileW;
        rect.VMin = y * tileH;
        rect.UMax = rect.UMin + tileW;
        rect.VMax = rect.VMin + tileH;

        return rect;
    }

    std::shared_ptr<Texture> TextureAtlas::GetTexture() {
        return m_Texture;
    }

}