#include "Texture.h"

#include <algorithm>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <slog/slog.h>

namespace Necrosis {

Texture::Texture() : _filePath(""), _width(0), _height(0), _bppx(0), _texture(0) {}

Texture::Texture(std::string path, int width, int height, int bppx, uint32_t texture, TextureID id)
        : _filePath(path), _width(width), _height(height)
        , _bppx(bppx), _texture(texture), _id(id) {}

void Texture::bind(const uint8_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _texture);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
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


TextureID TextureManager::makeTextureFromFile(const std::string &path) {
    // check if the texture is already loaded in which case return its id
    for (auto &tex : instance._textures) {
        if (tex._filePath == std::filesystem::path(path)) {
            return tex._id;
        }
    }

    // if the texture wasn't loaded before, do it
    int bppx = 0;
    int width = 0;
    int height = 0;
    uint32_t texture = 0;
    uint8_t* texBuffer = nullptr;

    stbi_set_flip_vertically_on_load(true);
    texBuffer = stbi_load(path.c_str(), &width, &height, &bppx, 4);
    if (!texBuffer) {
        slog::error(
            "Failed to load image at {} :\n{}", path, stbi_failure_reason()
        );
        return -1; // TODO: make a default texture and return it in case of failure
    }
    
    bppx = 4;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexImage2D(GL_TEXTURE_2D, 0,
    // _bppx == 4 ? GL_RGBA : GL_RGB, // an attempt to simply support multiple formats (not fool proof)
    // _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _texBuffer);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(texBuffer);
    texBuffer = nullptr;

    TextureID id = instance._textures.size();
    instance._textures.emplace_back(Texture(path, width, height, bppx, texture, id));
    
    return id;
}

void TextureManager::bind(TextureID id, const u8 slot) {
    for (auto &tex : instance._textures) {
        if (tex._id == id) {
            tex.bind(slot);
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

}
