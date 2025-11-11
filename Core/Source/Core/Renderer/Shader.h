#pragma once

#include <glm/glm.hpp>

#include <filesystem>
#include <array>

namespace Renderer {

    class Shader {
    public:
        Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        ~Shader();

        void Use();

        void SetInt(const char* key, const int& value) const;
        void SetBool(const char* key, const bool& value) const;
        void SetFloat(const char* key, const float& value) const;
        void SetMat4(const char* key, const glm::mat4& value) const;
        void SetVec3(const char* key, const glm::vec3& value) const;

    private:
        uint32_t m_Handle = 0;
    };

    uint32_t CreateGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

}