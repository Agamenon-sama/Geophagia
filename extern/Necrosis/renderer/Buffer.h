#pragma once

#include <vector>

#include <Common.h>

#define GL_FLOAT 0x1406
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_BYTE 0x1401
#define GL_FALSE 0
#define GL_TRUE 1

namespace Necrosis {

class VertexBuffer {
public:
    VertexBuffer() = default;
    VertexBuffer(const void* data, u32 size, u32 type = GL_FLOAT);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    void setData(const void* data, u32 size);

    u32 count() const;

private:
    u32 _id;

    u32 _count;
};

struct VertexBufferElement {
    u32 type;
    u32 count;
    u8 normalized;

    static u32 getSizeofType(u32 type) {
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


    void push(u32 type, u32 count) {
        uint8_t normalized = GL_TRUE;
        if(type == GL_UNSIGNED_BYTE) {
            normalized = GL_FALSE;
        }

        _elements.push_back({type, count, normalized});
        _stride += VertexBufferElement::getSizeofType(type) * count;
    }

    const std::vector<VertexBufferElement> &getElements() const { return _elements; }
    u32 getStride() const { return _stride; }

private:
    std::vector<VertexBufferElement> _elements;
    u32 _stride;
};

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer &vbo, const VertexBufferLayout &layout);

    void bind() const;
    void unbind() const;
private:
    u32 _id;
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    IndexBuffer(const u32* data, u32 count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    void setData(const u32* data, u32 count);

    u32 getCount() const;

private:
    u32 _id;
    u32 _count;
};

}
