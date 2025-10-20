#include "Terrain.h"

namespace Geophagia {

Terrain::Terrain() : _width(0), _depth(0) {}

Terrain::Terrain(const u32 width, const u32 depth) : _width(width), _depth(depth) {
    _heights.resize(_width * _depth);

    // for (u32 i = 0; i < _width * _depth; ++i) {
    //     _heights[i] = 0.0f;
    // }

    for (u32 z = 0; z < depth; z++) {
        for (u32 x = 0; x < width; x++) {
            _heights[z * width + x] = 3.f * std::sin(0.33f * x) + 3.f;
        }
    }

    _renderer = std::make_unique<TerrainRenderer>();
    _renderer->updateBuffers(_heights, _width, _depth);
}

Terrain::~Terrain() {

}

void Terrain::render() const {
    _renderer->render();
}

}