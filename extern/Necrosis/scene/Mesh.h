#pragma once

#include "../renderer/Buffer.h"
#include "../renderer/Texture.h"
// #include "../renderer/Shader.h"
#include "../renderer/Renderer.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>
// #include <filesystem>

namespace Necrosis {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh : public Renderable {
public:
    static Mesh makeQuad();
    static Mesh makeCube();
    // static std::shared_ptr<Mesh> fromFile(const std::filesystem::path &path);

    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<TextureID>  textures;
    
    
    Mesh() = default;
    Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds, std::vector<TextureID> texs);
    virtual ~Mesh() {}

    virtual std::shared_ptr<VertexArray> getVertexArray() const { return _vao; }
    virtual std::shared_ptr<VertexBuffer> getVertexBuffer() const { return _vbo; }
    virtual std::shared_ptr<IndexBuffer> getIndexBuffer() const { return _ebo; }

    void render() const override;

protected:
    std::shared_ptr<VertexArray>  _vao;
    std::shared_ptr<VertexBuffer> _vbo;
    std::shared_ptr<IndexBuffer>  _ebo;

protected:
    void _setupBuffers();
};

}
