#pragma once

#include "HeightmapGenerator.h"

namespace Geophagia {
/**
 * @brief Generates heightmaps using Perlin noise and fractal Brownian motion.
 */
class FbmGenerator : public HeightmapGenerator {
public:
    FbmGenerator() = default;
    FbmGenerator(Terrain *terrain);
    virtual ~FbmGenerator() override = default;

    /**
     * @brief Renders the widgets to set the parameters of the generator
     * and calls the generation function
     */
    void uiRender() override;

private:
    int _numOctaves; ///< @brief Describe the amount of details in the heightmap
    float _powerScaler; ///< @brief Used to accentuate the distance between the peaks and flats

    std::array<glm::vec2, 256> _gradients;
    std::array<u8, 512> _permutationTable;

    void _generateHeightmap();
    [[nodiscard]]
    int _hash(const int x, const int y) const;
    float _sample(glm::vec2 point) const;
};
}

