#pragma once

#include "HeightmapGenerator.h"

namespace Geophagia {
class FbmGenerator : public HeightmapGenerator {
public:
    FbmGenerator() = default;
    FbmGenerator(Terrain *terrain);
    virtual ~FbmGenerator() override = default;

    void uiRender() override;

private:
    int _numOctaves;

    std::array<glm::vec2, 256> _gradients;
    std::array<u8, 512> _permutationTable;

    void _generateHeightmap();
    [[nodiscard]]
    int _hash(const int x, const int y) const;
    float _sample(glm::vec2 point) const;
};
}

