#pragma once

#include <Common.h>

#include "../Terrain.h"

namespace Geophagia {
/**
 * @brief Interface for all the heightmap generator
 */
class HeightmapGenerator {
public:
    HeightmapGenerator() = default;
    HeightmapGenerator(Terrain *terrain) : _terrain(terrain) {}
    virtual ~HeightmapGenerator() = default;

    /**
     * @brief Renders the UI components for the heightmap generator.
     *
     * It should at least render input fields for the generator inputs and a generate button.
     */
    virtual void uiRender() = 0;

protected:
    Terrain *_terrain = nullptr;

    u64 _seed = 0;
};
}
