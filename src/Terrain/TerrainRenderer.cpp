#include "TerrainRenderer.h"

#include <glad/glad.h>

#include <Necrosis/scene/Mesh.h>

namespace Geophagia {

TerrainRenderer::TerrainRenderer() {
    _vao = std::make_unique<Necrosis::VertexArray>();
    _vao->bind();
    _vbo = std::make_unique<Necrosis::VertexBuffer>(nullptr, 0, GL_FLOAT);
    _vbo->bind();
    _ibo = std::make_unique<Necrosis::IndexBuffer>(nullptr, 0);
    _ibo->bind();

    Necrosis::VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    layout.push(GL_FLOAT, 3);
    layout.push(GL_FLOAT, 2);
    _vao->addBuffer(*_vbo, layout);

    _vao->unbind();
}

TerrainRenderer::~TerrainRenderer() {
}



void TerrainRenderer::render() const {
    _vao->bind();
    // _vbo->bind();
    // _ibo->bind();

    // glDrawArrays(GL_POINTS, 0, _vbo->count());
    glDrawElements(GL_TRIANGLES, _ibo->getCount(), GL_UNSIGNED_INT, 0);
}

void TerrainRenderer::updateBuffers(const std::vector<float> &heights, const u32 width, const u32 depth) const {
    if (width == 0 || depth == 0) { return; }
    slog::debug("creating terrain buffers with width and depth {}x{}", width, depth);

    // create the buffers that will be uploaded to the GPU
    std::vector<Necrosis::Vertex> vertices;
    std::vector<u32> indices;

    vertices.resize(width * depth);
    u32 numQuads = (width - 1) * (depth - 1);
    indices.resize(numQuads * 6);

    // generate vertex data
    size_t index = 0;
    for (u32 z = 0; z < depth; z++) {
        for (u32 x = 0; x < width; x++) {
            float y = heights[z * width + x];
            // TODO: put a real normal vector  and texture coordinates
            vertices[index] = Necrosis::Vertex(
                {static_cast<i32>(x) - static_cast<i32>(width) / 2, y, static_cast<i32>(z) - static_cast<i32>(depth) / 2}, glm::vec3(0.f, 1.f,0.f), glm::vec2(0.f)
            );
            index++;
        }
    }
    assert(index == vertices.size() && "error when populating the vertices buffer for the terrain");

    // generate index data
    index = 0;
    for (u32 z = 0; z < depth - 1; z++) {
        for (u32 x = 0; x < width - 1; x++) {
            // here we are at the base of a quad and the following
            // are the indices of the quad vertices
            u32 bottomLeft = z * width + x;
            u32 bottomRight = z * width + x + 1;
            u32 topLeft = (z + 1) * width + x;
            u32 topRight = (z + 1) * width + x + 1;

            // top left triangle
            indices[index++] = bottomLeft;
            indices[index++] = topRight;
            indices[index++] = topLeft;

            // bottom right triangle
            indices[index++] = bottomRight;
            indices[index++] = topRight;
            indices[index++] = bottomLeft;
        }
    }
    assert(index == indices.size() && "error when populating the indices buffer for the terrain");

    // send data to the GPU
    _vao->bind();
    _vbo->setData(vertices.data(), vertices.size() * sizeof(Necrosis::Vertex));
    _ibo->setData(indices.data(), indices.size());
    _vao->unbind();
}

}
