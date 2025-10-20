#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <Common.h>

namespace Necrosis {

using TextureID = i32;

class Texture {
public:
    Texture();

    void bind(const u8 slot = 0) const;
    void unbind() const;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    const std::filesystem::path& getFileName() const { return _filePath; }
    u32 getOpenglID() const { return _texture; }

    friend class TextureManager;

private:
    Texture(std::string path, int width, int height, int bppx, u32 texture, TextureID id);

    std::filesystem::path _filePath;
    int _width, _height, _bppx;
    uint32_t _texture; ///< OpenGL id
    TextureID _id; ///< TextureManager id
};


class TextureManager {
public:
    TextureManager(const TextureManager&) = delete;

    static void destroyAll();

    static TextureID makeTextureFromFile(const std::string &path);

    static Texture& getTextureFromID(const TextureID id);
    static void removeTexture(TextureID id);

    static void bind(TextureID id, const u8 slot = 0);
    static void unbind(TextureID id);

private:
    TextureManager() {}
    ~TextureManager();

private:
    static TextureManager instance;

    std::vector<Texture> _textures;
};

}
