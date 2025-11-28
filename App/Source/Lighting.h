#pragma once

#include <glm/glm.hpp>

namespace Lighting {

    struct Sun {
        glm::vec3 AmbientColor;
        glm::vec3 Color;
        glm::vec3 Direction;
    };

}