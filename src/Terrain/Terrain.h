#pragma once

#include <vector>
#include <memory>
#include <filesystem>

#include <Common.h>
#include <Necrosis/renderer/Renderer.h>
#include <Necrosis/renderer/Texture.h>

#include "TerrainRenderer.h"

namespace Geophagia {
/**
 * @brief Heightmap based terrain
 *
 * This class store a heightmap and its metadata and also contains
 * the tools to load and save the heightmaps. It should also be used
 * to render the terrain, but the rendering work is done in another class.
 */
class Terrain : public Necrosis::Renderable {
public:
    Terrain();
    Terrain(const u32 width, const u32 depth);
    virtual ~Terrain();

    void render() const override; ///< @brief renders the terrain
    /**
     * @brief Loads a new terrain from the passed values
     *
     * This function performs a validity check on the size of the terrain before
     * updating the heightmap and then resets the GPU buffers for rendering.
     *
     * @param heights vector of the elevation values of the heightmap
     * @param width width of the terrain
     * @param depth depth of the terrain
     * @return true on success and false on failure
     */
    bool loadRawFromMemory(const std::vector<f32> &heights, const u32 width, const u32 depth);

    /**
     * @brief Loads the heightmap from a raw file
     *
     * The file format *MUST* be:
     * u32 for the width, u32 for the depth and then all the elevation values
     * as 32bit floats. The number of elevation values must be width * depth.
     * The data *MUST* be in little endian.
     *
     * @param path path of the input file
     * @return true on success and false on failure
     */
    bool loadRawFromFile(const std::filesystem::path &path);

    /**
     * @brief Loads the heightmap from an image
     *
     * The supported file formats are png and jpeg.
     * Others formats are supported but not advised.
     * Ideally, use 1 channel per pixel.
     *
     * @param path path of the input file
     * @return true on success and false on failure
     */
    bool loadImageFromFile(const std::filesystem::path &path);

    bool saveAsPng(const std::filesystem::path &path) const;
    bool saveAsRaw(const std::filesystem::path &path) const;

    /**
     * @brief Draws the view of the heightmap as an image
     */
    void uiDrawHeightmapTexture() const;

    u32 getWidth() const { return _width; }
    u32 getDepth() const { return _depth; }
    // const u32* getHeightMap() const { return _heights; }

private:
    u32 _width;
    u32 _depth;

    std::vector<f32> _heights;
    Necrosis::TextureID _imageView;

    std::unique_ptr<TerrainRenderer> _renderer = nullptr;

    /**
     * @brief updates the content of `_texture` on the gpu side with the new
     * height values
     */
    void _updateImageView() const;
};
}
