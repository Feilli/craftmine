#include "Intersects.h"

namespace Intersects {

    bool RayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const AABB& boundBox, float& tNear) {
        float tmin = -FLT_MAX;
        float tmax =  FLT_MAX;

        for (int i = 0; i < 3; i++) {
            if (rayDirection[i] != 0.0f) {
                float t1 = (boundBox.MinBound[i] - rayOrigin[i]) / rayDirection[i];
                float t2 = (boundBox.MaxBound[i] - rayOrigin[i]) / rayDirection[i];

                if (t1 > t2) std::swap(t1, t2);

                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);

                if (tmax < tmin)
                    return false;
            }
            else { // Ray is parallel to axis
                if (rayOrigin[i] < boundBox.MinBound[i] || rayOrigin[i] > boundBox.MaxBound[i])
                    return false; // Outside slab
            }
        }

        // If tmin < 0 and tmax > 0 → origin is inside the box → intersection occurs at tmax
        tNear = (tmin >= 0.0f) ? tmin : tmax;

        return tNear >= 0.0f;
    }

    bool RayFace(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& model, FaceHit& hit) {
        // trasnform ray into cube local space
        glm::mat4 inverseModel = glm::inverse(model);
        glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localDirection = glm::normalize(glm::vec3(inverseModel * glm::vec4(rayDirection, 0.0f)));

        float tMin = 10.f;
        FaceDirection hitDirection = FaceDirection::NO_FACE;
        glm::vec3 hitNormal(0.0f);

        Face faces[] = {
            {{  1.0f,  0.0f,  0.0f }, 0.5f, FaceDirection::RIGHT}, // right
            {{ -1.0f,  0.0f,  0.0f }, 0.5f, FaceDirection::LEFT}, // left
            {{  0.0f,  1.0f,  0.0f }, 0.5f, FaceDirection::TOP}, // top
            {{  0.0f, -1.0f,  0.0f }, 0.5f, FaceDirection::BOTTOM}, // bottom
            {{  0.0f,  0.0f,  1.0f }, 0.5f, FaceDirection::FRONT}, // front
            {{  0.0f,  0.0f, -1.0f }, 0.5f, FaceDirection::BACK} // back
        };

        for(const Face& f : faces) {
            float denom = glm::dot(f.Normal, localDirection);

            if(fabs(denom) < 1e-6f) {
                continue;
            }

            float t = (f.D - glm::dot(f.Normal, localOrigin)) / denom;

            if(t < 0.0f) {
                continue;
            }

            glm::vec3 hit = localOrigin + t * localDirection;

            // check bound on other two axes
            if(fabs(hit.x) <= 0.5f + 1e-6f &&
                fabs(hit.y) <= 0.5f + 1e-6f &&
                fabs(hit.z) <= 0.5f + 1e-6f)  {

                if(f.Direction == FaceDirection::RIGHT || f.Direction == FaceDirection::LEFT) {
                    if(fabs(hit.y) > 0.5f || fabs(hit.z) > 0.5f) {
                        continue;
                    }
                }

                if(f.Direction == FaceDirection::TOP || f.Direction == FaceDirection::BOTTOM) {
                    if(fabs(hit.x) > 0.5f || fabs(hit.z) > 0.5f) {
                        continue;
                    }
                }
                
                if(f.Direction == FaceDirection::FRONT || f.Direction == FaceDirection::BACK) {
                    if(fabs(hit.x) > 0.5f || fabs(hit.y) > 0.5f) {
                        continue;
                    }
                }

                if(t < tMin) {
                    tMin = t;
                    hitDirection = f.Direction;
                    hitNormal = f.Normal;
                }
            }
        }

        if(hitDirection != FaceDirection::NO_FACE) {
            hit.T = tMin;
            hit.Direction = hitDirection;
            hit.Normal = glm::mat3(model) * hitNormal;
            return true;
        }

        return false;
    }

    bool AABBFrustum(const Frustum& frustum, const AABB& box) {
        for(int i = 0; i < 6; i++) {
            const Face& p = frustum.Faces[i];

            glm::vec3 positive;

            positive.x = (p.Normal.x >= 0) ? box.MaxBound.x : box.MinBound.x;
            positive.y = (p.Normal.y >= 0) ? box.MaxBound.y : box.MinBound.y;
            positive.z = (p.Normal.z >= 0) ? box.MaxBound.z : box.MinBound.z;

            float distance = glm::dot(p.Normal, positive) + p.D;

            if(distance < 0) {
                return false;
            }
        }

        return true;
    }

}