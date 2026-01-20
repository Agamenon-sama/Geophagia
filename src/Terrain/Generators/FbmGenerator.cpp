#include "FbmGenerator.h"

#include <random>

#include <imgui/imgui.h>

namespace Geophagia {
FbmGenerator::FbmGenerator(Terrain *terrain) : HeightmapGenerator(terrain), _numOctaves(3), _powerScaler(1.f) {}

void FbmGenerator::uiRender() {
    ImGui::Begin("Fbm Generator");
        ImGui::InputScalar("Seed:", ImGuiDataType_U64, &_seed);
        ImGui::InputInt("Number of octaves:", &_numOctaves);
        ImGui::SliderFloat("Power scale", &_powerScaler, 0.1f, 3.f);
        if (ImGui::Button("Generate")) {
            _generateHeightmap();
        }
    ImGui::End();
}

int FbmGenerator::_hash(const int x, const int y) const {
    return _permutationTable[_permutationTable[(x & 0xff)] + (y & 0xff)];
}

void FbmGenerator::_generateHeightmap() {
    if (!_terrain) {
        slog::warning("No terrain was assigned to this heightmap generator");
        return;
    }

    // clamp number of octaves for UX reasons ;)
    if (_numOctaves < 1) _numOctaves = 1;
    else if (_numOctaves > 8) _numOctaves = 8;

    // initialise perlin noise generator
    std::mt19937 mt(_seed);
    std::uniform_real_distribution<float> distf;
    std::uniform_int_distribution<int> disti(0, 255);
    auto rng = std::bind(distf, mt);
    auto rngi = std::bind(disti, mt);

    for (auto &g : _gradients) {
        float r = rng() * 2.f * std::numbers::pi_v<float>;
        g = glm::vec2(std::cos(r), std::sin(r));
    }

    for (u32 i = 0; i < 256; i++) {
        _permutationTable[i] = i;
    }
    for (u32 i = 0; i < 256; i++) {
        u32 j = rngi();
        std::swap(_permutationTable[i], _permutationTable[j]);
        _permutationTable[i + 256] = _permutationTable[i];
    }


    u32 width = _terrain->getWidth();
    u32 depth = _terrain->getDepth();

    std::vector<f32> heights(width * depth);

    float minVal = 1000.f;
    float maxVal = -1000.f;

    for (u32 z = 0; z < depth; z++) {
        for (u32 x = 0; x < width; x++) {
            float frequency = 0.005f;
            float amplitude = 1.f;

            heights[z * width + x] = 0.f;

            for (int oct = 0; oct < _numOctaves; oct++) {
                heights[z * width + x] += amplitude * ((_sample(glm::vec2(x, z) * frequency) + 1.f) * 0.5f);
                amplitude *= 0.5f;
                frequency *= 2.f;
            }

            // search for the min and max values in the heightmap
            float noiseSum = heights[z * width + x];
            if (noiseSum < minVal) minVal = noiseSum;
            if (noiseSum > maxVal) maxVal = noiseSum;
        }
    }

    float range = maxVal - minVal;

    for (u32 i = 0; i < width * depth; i++) {
        // normalize between the lowest and highest value
        float normalized = (heights[i] - minVal) / range;

        float shaped = std::pow(normalized, _powerScaler);

        heights[i] = shaped * 255.0f;
    }

    _terrain->loadRawFromMemory(heights, width, depth);
}

inline float fade(const float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
}
inline float lerp(const float p, const float q, const float t) { return p * (1 - t) + q * t; }

float FbmGenerator::_sample(glm::vec2 point) const {
    // coordinates on the grid
    int x0 = static_cast<int>(std::floor(point.x)) & 0xff;
    int y0 = static_cast<int>(std::floor(point.y)) & 0xff;

    int x1 = (x0 + 1) & 0xff;
    int y1 = (y0 + 1) & 0xff;

    float u = point.x - x0;
    float v = point.y - y0;

    // corner gradients
    const glm::vec2 grad00 = _gradients[_hash(x0, y0)];
    const glm::vec2 grad01 = _gradients[_hash(x1, y0)];
    const glm::vec2 grad10 = _gradients[_hash(x0, y1)];
    const glm::vec2 grad11 = _gradients[_hash(x1, y1)];

    // vectors from corners to points
    const glm::vec2 p00 = glm::vec2(u, v);
    const glm::vec2 p01 = glm::vec2(u-1, v);
    const glm::vec2 p10 = glm::vec2(u, v-1);
    const glm::vec2 p11 = glm::vec2(u-1, v-1);

    u = fade(u);
    v = fade(v);

    float a = lerp(dot(grad00, p00), dot(grad01, p01), u);
    float b = lerp(dot(grad10, p10), dot(grad11, p11), u);

    return lerp(a, b, v);
}



} // Geophagia