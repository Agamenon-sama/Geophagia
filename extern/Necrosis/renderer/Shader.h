#pragma once

#include <glm/glm.hpp>

#include <slog/slog.h>


#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace Necrosis {

class Shader {
public:
    static std::shared_ptr<Shader> makeFromFile(const std::filesystem::path &path);
    static std::shared_ptr<Shader> makeFromString(const std::string &source, const std::string &name = "");

    Shader() = default;
    ~Shader();
    // Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;
    int getUniformLocation(const std::string &name);
    void setUniform(const std::string &name, bool value);
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, glm::vec3 value);
    void setUniform(const std::string &name, glm::mat4 value);
private:
    static inline slog::Error _log;

    Shader(uint32_t id);

    uint32_t _id;
    std::unordered_map<std::string, int> _uniformLocations;
};

}
