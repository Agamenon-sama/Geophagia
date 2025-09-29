#pragma once

#include <cstdint>

namespace Necrosis {

class Framebuffer {
public:
    Framebuffer();

    void bind() const;
    void unbind() const;
    void bindTexture(const uint8_t slot = 0) const;
private:
    uint32_t _id;
    uint32_t _texture;
};

}
