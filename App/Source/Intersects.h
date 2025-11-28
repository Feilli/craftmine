#pragma once

#include <glm/glm.hpp>

#include <numeric>

namespace Intersects {

    struct AABB {
        glm::vec3 MinBound;
        glm::vec3 MaxBound;
    };

    enum FaceDirection {
        NO_FACE = -1,
        FRONT,
        BACK,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

    struct Face {
        glm::vec3 Normal;
        float D;
        FaceDirection Direction;
    };

    struct Frustum {
        Face Faces[6];
    };

    struct FaceHit {
        float T = 0.0f; // distance along the ray
        FaceDirection Direction = FaceDirection::NO_FACE;
        glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    Frustum GetFrustumFromViewProjectionMatrix(const glm::mat4& matrix);
    bool RayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const AABB& boundBox, float& tNear);
    bool RayFace(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& model, FaceHit& hit);
    bool AABBFrustum(const Frustum& frustum, const AABB& box);

}