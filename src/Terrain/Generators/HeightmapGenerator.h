#pragma once

#include <Common.h>

#include "../Terrain.h"

namespace Geophagia {
class HeightmapGenerator {
public:
    HeightmapGenerator() = default;
    HeightmapGenerator(Terrain *terrain) : _terrain(terrain) {}
    virtual ~HeightmapGenerator() = default;

    virtual void uiRender() = 0;

protected:
    Terrain *_terrain = nullptr;

    u64 _seed = 0;
};
}
