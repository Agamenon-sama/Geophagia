#pragma once

#include <cstdint>
#include <vector>

#define GL_FLOAT 0x1406
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_BYTE 0x1401
#define GL_FALSE 0
#define GL_TRUE 1

namespace Necrosis {

class VertexBuffer {
public:
    VertexBuffer() = default;
    VertexBuffer(const void* data, uint32_t size, uint32_t type = GL_FLOAT);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    uint32_t count() const;
private:
    uint32_t _id;

    uint32_t _count;
};

struct VertexBufferElement {
    uint32_t type;
    uint32_t count;
    uint8_t normalized;

    static uint32_t getSizeofType(uint32_t type) {
        switch(type) {
        case GL_FLOAT:          return 4;
        case GL_UNSIGNED_INT:   return 4;
        case GL_UNSIGNED_BYTE:  return 1;
        default: return 0;
        }
    }
};


class VertexBufferLayout {
public:
    VertexBufferLayout() : _stride(0) {}


    void push(uint32_t type, uint32_t count) {
        uint8_t normalized = GL_TRUE;
        if(type == GL_UNSIGNED_BYTE) {
            normalized = GL_FALSE;
        }

        _elements.push_back({type, count, normalized});
        _stride += VertexBufferElement::getSizeofType(type) * count;
    }

    const std::vector<VertexBufferElement> &getElements() const { return _elements; }
    uint32_t getStride() const { return _stride; }

private:
    std::vector<VertexBufferElement> _elements;
    uint32_t _stride;
};

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer &vbo, const VertexBufferLayout &layout);

    void bind() const;
    void unbind() const;
private:
    uint32_t _id;
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    IndexBuffer(const uint32_t* data, uint32_t count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    uint32_t getCount() const;
private:
    uint32_t _id;
    uint32_t _count;
};

}
