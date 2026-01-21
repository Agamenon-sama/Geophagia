#pragma once

#include <memory>

#include <Common.h>
#include <Necrosis/renderer/Renderer.h>
#include <Necrosis/renderer/Buffer.h>

namespace Geophagia {
class TerrainRenderer : public Necrosis::Renderable {
public:
    TerrainRenderer();
    // TerrainRenderer();
    ~TerrainRenderer();

    void render() const override;

    void updateBuffers(const std::vector<float> &heights, const u32 width, const u32 depth, const float textureScale) const;

private:
    std::unique_ptr<Necrosis::VertexArray> _vao;
    std::unique_ptr<Necrosis::VertexBuffer> _vbo;
    std::unique_ptr<Necrosis::IndexBuffer> _ibo;
};
}  // Geophagia
