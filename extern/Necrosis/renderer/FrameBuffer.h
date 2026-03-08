#pragma once

#include <glm/vec4.hpp>

#include "Common.h"

namespace Necrosis {

enum class FramebufferType : u8 {
    Color,
    Depth
};

class Framebuffer {
public:
    Framebuffer(const glm::ivec4 &viewport, FramebufferType type = FramebufferType::Color);

    void bind() const;
    void unbind() const;
    void bindTexture(const u8 slot = 0) const;

    i32 getWidth() const;
    i32 getHeight() const;
    glm::ivec4 getViewport() const;
    u32 getTextureID() const;

private:
    u32 _id;
    u32 _texture;
    glm::ivec4 _viewport;
    glm::ivec4 _defaultViewport;
};

}
