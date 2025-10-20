#include "Buffer.h"

#include <cstddef>
#include <glad/glad.h>

namespace Necrosis {

VertexBuffer::VertexBuffer(const void* data, u32 size, u32 type) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    switch (type) {
    case GL_FLOAT:
    default: // todo: change later
        _count = size / (sizeof(float) * 8); // 8 is sizeof(Vertex)
        break;
    }
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &_id);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(const void* data, u32 size) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    _count = size / (sizeof(float) * 8); // I hate this
    unbind();
}

u32 VertexBuffer::count() const {
    return _count;
}

// =====

VertexArray::VertexArray() {
    glGenVertexArrays(1, &_id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &_id);
}

void VertexArray::addBuffer(const VertexBuffer &vbo, const VertexBufferLayout &layout) {
    vbo.bind();
    bind();
    uint64_t offset = 0;
    const auto &elements = layout.getElements();
    for(size_t i = 0; i < elements.size(); i++) {
        const auto element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, 
            element.normalized, layout.getStride(), (const void*)offset);
        offset += element.count * VertexBufferElement::getSizeofType(element.type);
    }
}


void VertexArray::bind() const {
    glBindVertexArray(_id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

// =====

IndexBuffer::IndexBuffer(const u32* data, u32 count) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, GL_STATIC_DRAW);

    _count = count;
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &_id);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setData(const u32* data, u32 count) {
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, GL_STATIC_DRAW);
    _count = count;
    unbind();
}

u32 IndexBuffer::getCount() const { return _count; }

}
