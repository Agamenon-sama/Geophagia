#include "Texture.h"

#include <algorithm>
#include <cassert>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <slog/slog.h>

namespace Necrosis {

Texture::Texture() : _filePath(""), _width(0), _height(0)
    , _format(PixelFormat::RGBA), _texture(0) {}

Texture::Texture(std::string path, int width, int height, PixelFormat format, uint32_t texture, TextureID id)
        : _filePath(path), _name(_filePath.stem().string()), _width(width), _height(height)
        , _format(format), _texture(texture), _id(id) {}

void Texture::bind(const uint8_t slot, const u8 unit) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glBindTextureUnit(unit, _texture);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::updateTexture(const u8 *data, int width, int height, PixelFormat pixelFormat) {
    if (!data) { return; }
    _width = width;
    _height = height;
    bind();

    // TODO: if resolution and format didn't change, use this instead (faster)
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, format, GL_UNSIGNED_BYTE, data);

    u32 format = 0;
    u32 internalFormat = 0;
    switch (pixelFormat) {
    case PixelFormat::RGBA:
        format = GL_RGBA;
        internalFormat = GL_RGBA8;
        break;
    case PixelFormat::RGB:
        format = GL_RGB;
        internalFormat = GL_RGB8;
        break;
    case PixelFormat::Luminance:
        format = GL_RED;
        internalFormat = GL_R8;
        break;
    default:
        slog::warning("Invalid pixel format specified");
        return;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    // if (pixelFormat == PixelFormat::Luminance) {
    //     i32 swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
    //     glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    // }

    unbind();
}

TextureManager TextureManager::instance;

TextureManager::~TextureManager() {
    // for (auto &tex : instance._textures) {
    //     // feels bad to call glDeleteTextures to delete textures 1 by 1 in a loop
    //     // when the function can delete them all at once if they were contiguous
    //     // I guess my code isn't Data Oriented (TM) enough (╥﹏╥)
    //     glDeleteTextures(1, &tex._texture);
    // }
}

void TextureManager::destroyAll() {
    for (auto &tex : instance._textures) {
        // feels bad to call glDeleteTextures to delete textures 1 by 1 in a loop
        // when the function can delete them all at once if they were contiguous
        // I guess my code isn't Data Oriented (TM) enough (╥﹏╥)
        glDeleteTextures(1, &tex._texture);
    }
}

TextureID TextureManager::makeTextureFromMemory(const u8 *data, int width, int height, PixelFormat format) {
    if (!data) { return -1; }

    u32 texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // if GL_RED, the user wants a grayscale 1 channel texture
    if (format == PixelFormat::Luminance) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        i32 swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }
    else if (format == PixelFormat::RGB) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (format == PixelFormat::RGBA) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        slog::error("Invalid pixel format specified");
        return -1;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    TextureID id = instance._textures.size();
    Texture tex;
    tex._id = id;
    tex._texture = texture;
    tex._width = width;
    tex._height = height;
    instance._textures.emplace_back(tex);

    return id;
}

TextureID TextureManager::makeTextureFromFile(const std::filesystem::path &path, PixelFormat format) {
    // check if the texture is already loaded in which case return its id
    for (auto &tex : instance._textures) {
        if (tex._filePath == path) {
            return tex._id;
        }
    }

    // if the texture wasn't loaded before, do it
    int bppx = 0;
    int width = 0;
    int height = 0;
    uint32_t texture = 0;
    uint8_t* texBuffer = nullptr;

    int desiredBpp = 0;
    switch (format) {
        case PixelFormat::Luminance: desiredBpp = 1; break;
        case PixelFormat::RGB: desiredBpp = 3; break;
        case PixelFormat::RGBA: desiredBpp = 4; break;
        case PixelFormat::Unknown:
        default:
            desiredBpp = 0;
    }

    stbi_set_flip_vertically_on_load(true);
    texBuffer = stbi_load(path.c_str(), &width, &height, &bppx, desiredBpp);
    if (!texBuffer) {
        slog::error(
            "Failed to load image at {} :\n{}", path.string(), stbi_failure_reason()
        );
        return -1; // TODO: make a default texture and return it in case of failure
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (bppx == 1) {
        format = PixelFormat::Luminance;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, texBuffer);
        i32 swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }
    else if (bppx == 3) {
        format = PixelFormat::RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texBuffer);
    }
    else if (bppx == 4) {
        format = PixelFormat::RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer);
    }
    else {
        slog::error("Error loading image '{}': Pixel format unsupported", path.string());
        return -1;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    glObjectLabel(GL_TEXTURE, texture, -1, path.stem().c_str());
    
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(texBuffer);
    texBuffer = nullptr;

    TextureID id = instance._textures.size();
    instance._textures.emplace_back(Texture(path, width, height, format, texture, id));
    
    return id;
}

void TextureManager::bind(TextureID id, const u8 slot, const u8 unit) {
    for (auto &tex : instance._textures) {
        if (tex._id == id) {
            tex.bind(slot, unit);
        }
    }
}

void TextureManager::unbind(TextureID id) {
    for (auto &tex : instance._textures) {
        if (tex._id == id) {
            tex.unbind();
        }
    }
}

Texture &TextureManager::getTextureFromID(const TextureID id) {
    // TODO: if id doesn't exist return some default texture
    assert(id >= 0 && static_cast<size_t>(id) < instance._textures.size() && "Texture does not exist");
    return instance._textures[id];
}


void TextureManager::removeTexture(TextureID id) {
    auto it = std::find_if(
        instance._textures.begin(), instance._textures.end(),
        [id](const Texture &t) { return t._id == id; }
    );

    // if found texture
    if (it != instance._textures.end()) {
        glDeleteTextures(1, &(it->_texture));
        instance._textures.erase(it);
    }
}

float TextureSampler::getMaxAnisotropySamples() {
    float samples = 0.f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &samples);
    return samples;
}

TextureSampler::TextureSampler(FilterType filter, WrapMode wrap, float anisotropySamples, std::string name)
    : _name(std::move(name)), _filter(filter), _wrap(wrap), _anisotropySamples(anisotropySamples) {

    glCreateSamplers(1, &_handle);
    glObjectLabel(GL_SAMPLER, _handle, -1, _name.c_str());

    switch (_filter) {
    case FilterType::Nearest:
        glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case FilterType::Linear:
        glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case FilterType::LinearMipmap:
    default:
        glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }

    switch (_wrap) {
    case WrapMode::Repeat:
        glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    case WrapMode::Mirror:
        glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;
    case WrapMode::Clamp:
    default:
        glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    }

    setAnisotropySamples(anisotropySamples);
}

TextureSampler::~TextureSampler() {
    if (_handle) {
        glDeleteSamplers(1, &_handle);
        _handle = 0;
    }
}

TextureSampler::TextureSampler(TextureSampler &&other) : _handle(other._handle), _name(std::move(other._name))
    , _filter(other._filter), _wrap(other._wrap), _anisotropySamples(other._anisotropySamples) {

    other._handle = 0;
}

TextureSampler &TextureSampler::operator=(TextureSampler &&other) {
    if (this != &other) {
        if (_handle) {
            glDeleteSamplers(1, &_handle);
        }

        _handle = other._handle;
        _name = std::move(other._name);
        _filter = other._filter;
        _wrap = other._wrap;
        _anisotropySamples = other._anisotropySamples;
        other._handle = 0;
    }
    return *this;
}


void TextureSampler::bind(u8 slot) const {
    glBindSampler(slot, _handle);
}


void TextureSampler::setAnisotropySamples(float anisotropySamples) {
    _anisotropySamples = anisotropySamples;
    if (_anisotropySamples > 1.f) {
        float maxSamples = getMaxAnisotropySamples();
        glSamplerParameterf(
            _handle, GL_TEXTURE_MAX_ANISOTROPY,
            std::min(maxSamples, _anisotropySamples)
        );
    }
}

}
