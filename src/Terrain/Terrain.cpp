#include "Terrain.h"

#include <fstream>

#include <stb/stb_image.h>

namespace Geophagia {

Terrain::Terrain() : _width(0), _depth(0), _renderer(std::make_unique<TerrainRenderer>()) {}

Terrain::Terrain(const u32 width, const u32 depth) : _width(width), _depth(depth), _renderer(std::make_unique<TerrainRenderer>()) {
    _heights.resize(_width * _depth);

    for (u32 i = 0; i < _width * _depth; ++i) {
        _heights[i] = 0.0f;
    }

    // for (u32 z = 0; z < depth; z++) {
    //     for (u32 x = 0; x < width; x++) {
    //         _heights[z * width + x] = 3.f * std::sin(0.33f * x) + 3.f;
    //     }
    // }

    _renderer->updateBuffers(_heights, _width, _depth);
}

Terrain::~Terrain() {

}

void Terrain::render() const {
    _renderer->render();
}

bool Terrain::loadRawFromFile(const std::filesystem::path &path) {
    std::fstream file(path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        slog::warning("Failed to open raw heightmap file '{}'", path.string());
        return false;
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    size -= 2 * sizeof(u32); // the 2 first ints are for the width and depth
    if (size <= 0) {
        slog::warning("The file size is invalid for heightmap file '{}'", path.string());
        return false;
    }
    if (size% sizeof(f32) != 0) {
        slog::warning("The heightmap file '{}' is invalid", path.string());
        return false;
    }

    if (!file.read(reinterpret_cast<char *>(&_width), sizeof(_width))) {
        slog::warning("Failed to read from heightmap file '{}'", path.string());
        return false;
    }
    if (!file.read(reinterpret_cast<char *>(&_depth), sizeof(_depth))) {
        slog::warning("Failed to read from heightmap file '{}'", path.string());
        return false;
    }
    if (_width <= 0 || _depth <= 0) {
        slog::warning(
            "Error loading heightmap '{}'\n"
            "The width and depth sizes of the map must be greater than 0",
            path.string()
        );
        return false;
    }
    if (_width * _depth * sizeof(f32) != static_cast<u64>(size)) {
        slog::warning(
            "Error loading heightmap '{}'\n"
            "the dimensions of the map don't match the file size",
            path.string()
        );
    }

    _heights.resize(_width * _depth, 2.f);
    if (!file.read(reinterpret_cast<char *>(_heights.data()), size)) {
        slog::warning("Failed to read from heightmap file '{}'", path.string());
        return false;
    }

    _renderer->updateBuffers(_heights, _width, _depth);
    return true;
}

bool Terrain::loadImageFromFile(const std::filesystem::path &path) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(false);
    u8 *imageBuffer = stbi_load(path.c_str(), &width, &height, &channels, 1);

    if (!imageBuffer) {
        slog::warning("Failed to load heightmap '{}'", path.string());
        return false;
    }

    if (width <= 0 || height <= 0) {
        slog::warning(
            "Error loading heightmap '{}'\n"
            "The width and depth sizes of the map must be greater than 0",
            path.string()
        );
        return false;
    }

    _width = width;
    _depth = height;

    _heights.resize(_width * _depth);
    for (u32 i = 0; i < _width * _depth; ++i) {
        _heights[i] = imageBuffer[i];
    }

    stbi_image_free(imageBuffer);
    _renderer->updateBuffers(_heights, _width, _depth);
    return true;
}

}