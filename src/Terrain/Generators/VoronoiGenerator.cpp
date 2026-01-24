#include "VoronoiGenerator.h"

#include <random>

#include <imgui/imgui.h>

namespace Geophagia {

VoronoiGenerator::VoronoiGenerator() : HeightmapGenerator(), _numCentroids(15) {}
VoronoiGenerator::VoronoiGenerator(Terrain *terrain) : HeightmapGenerator(terrain), _numCentroids(15) {}

VoronoiGenerator::~VoronoiGenerator() {}

void VoronoiGenerator::uiRender() {
    ImGui::Begin("Voronoi Generator");

        ImGui::InputScalar("Seed", ImGuiDataType_U64, &_seed);
        ImGui::InputInt("Number of centroids", &_numCentroids);
        if (ImGui::Button("Generate")) {
            _generateHeightmap();
        }

    ImGui::End();
}

void VoronoiGenerator::_generateHeightmap() {
    if (!_terrain) {
        slog::warning("No terrain was assigned to this heightmap generator");
        return;
    }

    if (_numCentroids < 1) {
        slog::warning("Not enough centroids to generate voronoi heightmap");
        return;
    }

    u32 width = _terrain->getWidth();
    u32 depth = _terrain->getDepth();

    std::mt19937_64 generator(_seed);
    std::uniform_real_distribution<float> dist(0, 1);

    std::vector<glm::vec3> centroids;

    for (int i = 0; i < _numCentroids; ++i) {
        float x = dist(generator) * width;
        float z = dist(generator) * depth;
        // we generate a random elevation for each centroid.
        // all the points close to it will have this elevation
        float elevation = dist(generator) * 256.f;

        centroids.emplace_back(x, elevation, z);
    }

    std::vector<f32> heights(width * depth);

    for (size_t i = 0; i < heights.size(); i++) {
        auto current = glm::vec2(i % width, i / width);
        float minD = glm::length(glm::vec2(0, 0) - glm::vec2(centroids[0].x, centroids[0].z));

        for (size_t j = 1; j < centroids.size(); j++) {
            auto c = centroids[j];

            float d = glm::length(current - glm::vec2(c.x, c.z));

            if (d < minD) {
                minD = d;
                heights[i] = c.y; // the elevation from the centroid
            }
        }
    }

    _terrain->loadRawFromMemory(heights, width, depth);
}



}
