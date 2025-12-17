#include "Terrain.h"

#include <fstream>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <imgui/imgui.h>
#include <Necrosis/renderer/Texture.h>
#include <Necrosis/Window.h>

#include "../UiComponents/Dialogs.h"

namespace Geophagia {

Terrain::Terrain() : _width(257), _depth(257), _renderer(std::make_unique<TerrainRenderer>()) {
    _heights.resize(_width * _depth, 0.f);

    _renderer->updateBuffers(_heights, _width, _depth);

    // set the image view of the heightmap
    std::vector<u8> image(_width * _depth);

    for (size_t i = 0; i < image.size(); i++) {
        image[i] = static_cast<u8>(_heights[i]);
    }

    _imageView = Necrosis::TextureManager::makeTextureFromMemory(image.data(), _width, _depth, GL_RED);
}

Terrain::Terrain(const u32 width, const u32 depth)
    : _width(width), _depth(depth)
    , _renderer(std::make_unique<TerrainRenderer>()) {

    _heights.resize(_width * _depth);

    for (u32 i = 0; i < _width * _depth; ++i) {
        _heights[i] = 0.0f;
    }

    // set the buffer that contains the heightmap mesh
    _renderer->updateBuffers(_heights, _width, _depth);

    // set the image view of the heightmap
    std::vector<u8> image(_width * _depth);

    for (u32 y = 0; y < _depth; y++) {
        for (u32 x = 0; x < _width; x++) {
            u32 cell = (x / 8 + y / 8) & 1;
            image[y * _width + x] = cell ? 255 : 0;
        }
    }

    _imageView = Necrosis::TextureManager::makeTextureFromMemory(image.data(), _width, _depth, GL_RED);
}

Terrain::~Terrain() {

}

void Terrain::render() const {
    _renderer->render();
}

bool Terrain::loadRawFromMemory(const std::vector<f32> &heights, const u32 width, const u32 depth) {
    if (width == 0 || depth == 0) {
        slog::warning("The heightmap width and depth has to be greater than 0");
        return false;
    }
    if (width * depth != heights.size()) {
        slog::warning("the size of the heightmap provided is invalid");
        return false;
    }

    _heights = heights; // NOTE: might use std::move but heights should be &&
    _width = width;
    _depth = depth;

    _renderer->updateBuffers(_heights, _width, _depth);
    _updateImageView();
    return true;
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

    _heights.resize(_width * _depth);
    if (!file.read(reinterpret_cast<char *>(_heights.data()), size)) {
        slog::warning("Failed to read from heightmap file '{}'", path.string());
        return false;
    }

    _renderer->updateBuffers(_heights, _width, _depth);
    _updateImageView();
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
    _updateImageView();
    return true;
}

void Terrain::_updateImageView() const {
    std::vector<u8> image(_width * _depth);

    for (size_t i = 0; i < image.size(); i++) {
        image[i] = static_cast<u8>(_heights[i]);
    }

    auto texture = Necrosis::TextureManager::getTextureFromID(_imageView);
    texture.updateTexture(image.data(), _width, _depth, GL_RED);
}

void Terrain::uiDrawHeightmapTexture() const {
    ImGui::Begin("Heightmap");

        ImGui::Text("Resolution: %dx%d", _width, _depth);
        ImGui::Image(
            Necrosis::TextureManager::getTextureFromID(_imageView).getOpenglID(),
            ImVec2(static_cast<float>(_width), static_cast<float>(_depth))
        );

        if (ImGui::Button("Save as raw")) {
            ImGui::OpenPopup("Save as raw");
        } ImGui::SameLine();
        if (ImGui::Button("Save as png")) {
            ImGui::OpenPopup("Save as png");
        }

        showSaveDialog("Save as png", [&](const char filename[]) {
            if (!saveAsPng(filename)) {
                std::string msg = std::format("Failed to write to file '{}'", filename);
                slog::warning(msg);
                Necrosis::Window::showWarningMessageBox(msg);
            }
        });

        showSaveDialog("Save as raw", [&](const char filename[]) {
            if (!saveAsRaw(filename)) {
                std::string msg = std::format("Failed to write to file '{}'", filename);
                slog::warning(msg);
                Necrosis::Window::showWarningMessageBox(msg);
            }
        });

    ImGui::End();
}

bool Terrain::saveAsPng(const std::filesystem::path &path) const {
    assert(_width * _depth == _heights.size() && "The heightmap size is invalid");
    std::vector<u8> image(_width * _depth);

    for (size_t i = 0; i < image.size(); i++) {
        image[i] = static_cast<u8>(_heights[i]);
    }

    if (!stbi_write_png(path.c_str(), _width, _depth, 1, image.data(), _width)) {
        return false;
    }

    return true;
}

bool Terrain::saveAsRaw(const std::filesystem::path &path) const {
    assert(_width * _depth == _heights.size() && "The heightmap size is invalid");

    std::fstream file(path, std::ios::binary | std::ios::out);
    if (!file.is_open()) {
        slog::warning("Failed to save raw heightmap file '{}'", path.string());
        return false;
    }

    file.write(reinterpret_cast<const char*>(&_width), sizeof(_width));
    file.write(reinterpret_cast<const char*>(&_depth), sizeof(_depth));

    for (size_t i = 0; i < _heights.size(); i++) {
        file.write(reinterpret_cast<const char*>(&_heights[i]), sizeof(f32));
    }

    return true;
}




}
