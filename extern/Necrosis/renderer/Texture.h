#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <Common.h>

namespace Necrosis {

/**
 * @enum PixelFormat
 * @brief The format of the pixel of the textures.
 */
enum class PixelFormat : u32 {
    Unknown, ///< @brief Undefined format. This can be a valid option depending on the function using it.
    Luminance, ///< @brief 8bit single channel.
    RGB, ///< @brief 8bit per channel RGB.
    RGBA ///< @brief 8bit per channel RGBA.
};

/**
 * @enum FilterType
 * @brief The method used to sample textures.
 */
enum class FilterType : u32 {
    Unknown,
    Nearest,
    Linear,
    LinearMipmap
};

/**
 * @enum WrapMode
 * @brief Defines how texture coordinates outside [0, 1] are handled.
 */
enum class WrapMode : u32 {
    Clamp,  ///< @brief Clamps coordinates to [0, 1], stretching edge pixels.
    Repeat, ///< @brief Tiles the texture by repeating it in both directions.
    Mirror  ///< @brief Tiles the texture, mirroring it every other repeat.
};


using TextureID = i32;

class Texture {
public:
    Texture();

    void bind(const u8 slot = 0, const u8 unit = 0) const;
    void unbind() const;

    void updateTexture(
        const u8 *data, int width, int height, PixelFormat pixelFormat = PixelFormat::RGBA
    );

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    const std::filesystem::path& getFileName() const { return _filePath; }
    u32 getOpenglID() const { return _texture; }

    friend class TextureManager;

private:
    Texture(std::string path, int width, int height, PixelFormat format, u32 texture, TextureID id);

    std::filesystem::path _filePath;
    std::string _name;

    int _width, _height;
    PixelFormat _format;

    u32 _texture; ///< OpenGL id
    TextureID _id; ///< TextureManager id
};


class TextureManager {
public:
    TextureManager(const TextureManager&) = delete;

    static void destroyAll();

    static TextureID makeTextureFromMemory(
        const u8 *data, int width, int height, PixelFormat format = PixelFormat::RGBA
    );
    static TextureID makeTextureFromFile(
        const std::filesystem::path &path, PixelFormat format = PixelFormat::Unknown
    );

    static Texture& getTextureFromID(const TextureID id);
    static void removeTexture(TextureID id);

    static void bind(TextureID id, const u8 slot = 0, const u8 unit = 0);
    static void unbind(TextureID id);

private:
    TextureManager() {}
    ~TextureManager();

private:
    static TextureManager instance;

    std::vector<Texture> _textures;
};

/**
 * @brief Contains the sampling information like the wrap mode and filter type
 */
class TextureSampler {
public:
    TextureSampler() = default;
    TextureSampler(FilterType filter, WrapMode wrap, float anisotropySamples = 1.f, std::string name = "Sampler");
    ~TextureSampler();

    TextureSampler(const TextureSampler&) = delete;
    TextureSampler& operator=(const TextureSampler&) = delete;
    TextureSampler(TextureSampler&&) noexcept;
    TextureSampler& operator=(TextureSampler&&) noexcept;

    /**
     * @return The maximum anisotropy samples supported by the platform.
     */
    static float getMaxAnisotropySamples();

    /**
     * @brief To apply a sampler to a texture, you have to bind both the sampler
     * and the texture to the same unit.
     * @param unit ID of the unit the sampler is to be bound to.
     */
    void bind(u8 unit = 0) const;

    /**
     * @brief Sets the number of anisotropy samples. The actual number applied is the minimum between
     * the desired number and the maximum supported by the platform.
     * @param anisotropySamples The number of anisotropy samples
     */
    void setAnisotropySamples(float anisotropySamples);

private:
    u32 _handle = 0;
    std::string _name;

    FilterType _filter = FilterType::LinearMipmap;
    WrapMode _wrap = WrapMode::Clamp;
    float _anisotropySamples = 1.f;
};

}
