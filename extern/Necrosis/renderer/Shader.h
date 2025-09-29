#pragma once

#include <glm/glm.hpp>

#include <slog/slog.h>


#include <string>
#include <memory>

namespace Necrosis {

class Shader {
public:
    static std::shared_ptr<Shader> makeFromFile(const std::string &path);
    static std::shared_ptr<Shader> makeFromString(const std::string &source);

    Shader() = default;
    ~Shader();
    // Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3f(const std::string &name, glm::vec3 value) const;
    void setMat4f(const std::string &name, glm::mat4 value) const;
private:
    static inline slog::Error _log;

    Shader(uint32_t id);

    uint32_t _id;
};

}
