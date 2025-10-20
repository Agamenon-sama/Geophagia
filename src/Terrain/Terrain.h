#pragma once

#include <vector>
#include <memory>

#include <Common.h>
#include <Necrosis/renderer/Renderer.h>

#include "TerrainRenderer.h"

namespace Geophagia {
class Terrain : public Necrosis::Renderable {
public:
    Terrain();
    Terrain(const u32 width, const u32 depth);
    virtual ~Terrain();

    void render() const override;

private:
    u32 _width;
    u32 _depth;

    std::vector<f32> _heights;

    std::unique_ptr<TerrainRenderer> _renderer = nullptr;
};
}
