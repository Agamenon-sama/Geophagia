#pragma once

#include "HeightmapGenerator.h"

namespace Geophagia {
class VoronoiGenerator : public HeightmapGenerator {
public:
    VoronoiGenerator();
    VoronoiGenerator(Terrain *terrain);
    ~VoronoiGenerator() override;

    void uiRender() override;

private:
    int _numCentroids;

    void _generateHeightmap();
};
}
