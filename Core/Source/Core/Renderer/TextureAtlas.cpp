#include "TextureAtlas.h"

namespace Renderer {

    TextureAtlas::TextureAtlas(const std::filesystem::path& path, int width, int height) {
        m_Texture = std::make_shared<Texture>(path);

        m_Width = width;
        m_Height = height;
    }

    TextureAtlas::~TextureAtlas() {

    }

    std::vector<glm::vec2> TextureAtlas::GetTileUV(int x, int y) const {
        float tileW = 1.0f / m_Width;
        float tileH = 1.0f / m_Height;

        // UVRect rect;
        float UMin = x * tileW;
        float VMin = y * tileH;
        float UMax = UMin + tileW;
        float VMax = VMin + tileH;

        std::vector<glm::vec2> uvs = {
            { UMin, VMax },
            { UMax, VMax },
            { UMax, VMin },
            { UMin, VMin }
        };

        return uvs;
    }

    std::shared_ptr<Texture> TextureAtlas::GetTexture() {
        return m_Texture;
    }

}