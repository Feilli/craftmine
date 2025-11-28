#pragma once

#include <glm/glm.hpp>

namespace Renderer {

    struct Vertex {
        glm::vec3 Position;
        glm::vec2 UVs;
        glm::vec3 Normal;
        uint8_t AO;
    };

}