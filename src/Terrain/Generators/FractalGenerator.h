#pragma once

#include "HeightmapGenerator.h"

namespace Geophagia {
/**
 * @brief Generates heightmaps using Perlin noise and fractal methods.
 */
class FractalGenerator : public HeightmapGenerator {
public:
    FractalGenerator() = default;
    FractalGenerator(Terrain *terrain);
    virtual ~FractalGenerator() override = default;

    /**
     * @brief Renders the widgets to set the parameters of the generator
     * and calls the generation function
     */
    void uiRender() override;

private:
    int _numOctaves; ///< @brief Describe the amount of details in the heightmap
    float _powerScaler; ///< @brief Used to accentuate the distance between the peaks and flats
    float _persistence; ///< @brief How much detail to keep from the higher octaves
    float _lacunarity; ///< @brief Controls the gap between the patterns

    std::array<glm::vec2, 256> _gradients;
    std::array<u8, 512> _permutationTable;

    /**
     * @brief Generates the height values and notifies the terrain
     * to update its buffers.
     * @param algo 0 for fbm, 1 for rmf
     */
    void _generateHeightmap(int algo);
    [[nodiscard]]
    int _hash(const int x, const int y) const;
    float _sample(glm::vec2 point) const;
};
}

