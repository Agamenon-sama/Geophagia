#include "TerrainRenderer.h"

#include <chrono>

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
    layout.push(GL_FLOAT, 3);
    _vao->addBuffer(*_vbo, layout);

    _vao->unbind();

    // _normalVao = std::make_unique<Necrosis::VertexArray>();
    // _normalVao->bind();
    // _normalVbo = std::make_unique<Necrosis::VertexBuffer>(nullptr, 0, GL_FLOAT);
    // _normalVbo->bind();

    // Necrosis::VertexBufferLayout normalLayout;
    // normalLayout.push(GL_FLOAT, 3);
    // normalLayout.push(GL_FLOAT, 3);
    // normalLayout.push(GL_FLOAT, 2);
    // _normalVao->addBuffer(*_normalVbo, normalLayout);
    //
    // _normalVao->unbind();
}

TerrainRenderer::~TerrainRenderer() {
}



void TerrainRenderer::render() const {
    _vao->bind();
    // _vbo->bind();
    // _ibo->bind();

    // glDrawArrays(GL_POINTS, 0, _vbo->count());
    glDrawElements(GL_TRIANGLES, _ibo->getCount(), GL_UNSIGNED_INT, 0);
    _vao->unbind();

     // glLineWidth(2.f);
     // _normalVao->bind();
     // glDrawArrays(GL_LINES, 0, _normalVbo->count());
     // glLineWidth(1.f);
}

[[nodiscard]]
glm::vec3 generateNormal(u32 x, u32 z, const std::vector<f32> &heights, u32 width, u32 depth) {
    expect((x < width) && (z < depth), "Invalid coordinate for normal generation");
    auto getHeight = [&](i32 x, i32 z) {
        x = std::clamp(x, 0, static_cast<i32>(width) - 1);
        z = std::clamp(z, 0, static_cast<i32>(depth) - 1);
        return heights[z * width + x];
    };

    std::array<glm::vec3, 6> neighbours = {
        glm::vec3(static_cast<i32>(x) + 1, getHeight(static_cast<i32>(x) + 1, static_cast<i32>(z)), static_cast<i32>(z)), // R
        glm::vec3(static_cast<i32>(x) + 1, getHeight(static_cast<i32>(x) + 1, static_cast<i32>(z) + 1), static_cast<i32>(z) + 1), // UR
        glm::vec3(static_cast<i32>(x), getHeight(static_cast<i32>(x), static_cast<i32>(z) - 1),  static_cast<i32>(z) - 1), // U
        glm::vec3(static_cast<i32>(x) - 1, getHeight(static_cast<i32>(x) - 1, static_cast<i32>(z)), static_cast<i32>(z)), // L
        glm::vec3(static_cast<i32>(x) - 1, getHeight(static_cast<i32>(x) - 1, static_cast<i32>(z) - 1), static_cast<i32>(z) - 1), // DL
        glm::vec3(static_cast<i32>(x), getHeight(static_cast<i32>(x), static_cast<i32>(z) + 1), static_cast<i32>(z) + 1) // D
    };

    glm::vec3 current(x, heights[z * width + x], z);
    glm::vec3 normal(0.f);

    for (int i = 0; i < 6; i++) {
        glm::vec3 v1 = neighbours[i] - current;
        glm::vec3 v2 = neighbours[(i + 1) % 6] - current;
        normal += glm::cross(v1, v2);
    }

    return glm::normalize(normal);
}

[[nodiscard]]
glm::vec3 generateNormalFast(u32 x, u32 z, const std::vector<f32> &heights, u32 width, u32 depth) {
    expect((x < width) && (z < depth), "Invalid coordinate for normal generation");
    auto getHeight = [&](i32 x, i32 z) {
        x = std::clamp(x, 0, static_cast<i32>(width) - 1);
        z = std::clamp(z, 0, static_cast<i32>(depth) - 1);
        return heights[z * width + x];
    };

    const float hL = getHeight(static_cast<i32>(x) - 1, static_cast<i32>(z));
    const float hR = getHeight(static_cast<i32>(x) + 1, static_cast<i32>(z));
    const float hU = getHeight(static_cast<i32>(x), static_cast<i32>(z) - 1);
    const float hD = getHeight(static_cast<i32>(x), static_cast<i32>(z) + 1);

    return glm::normalize(glm::vec3(hL - hR, 2.f, hU - hD));
}

void TerrainRenderer::updateBuffers(const std::vector<float> &heights, const u32 width, const u32 depth, const float textureScale, const float mapScale) const {
    if (width == 0 || depth == 0) { return; }

    // create the buffers that will be uploaded to the GPU
    std::vector<Necrosis::Vertex> vertices;
    std::vector<u32> indices;

    vertices.resize(width * depth);
    u32 numQuads = (width - 1) * (depth - 1);
    indices.resize(numQuads * 6);

    // std::vector<Necrosis::Vertex> normalLines;

    // generate vertex data
    size_t index = 0;
    for (u32 z = 0; z < depth; z++) {
        for (u32 x = 0; x < width; x++) {
            float y = heights[z * width + x];

            auto pos = glm::vec3((f32)x / (f32)width, y, (f32)z / (f32)depth);
            pos = glm::vec3(pos.x * 2.f - 1.f, pos.y, pos.z * 2.f - 1.f);
            pos *= glm::vec3((f32)mapScale, 1.f, (f32)mapScale);
            auto normal = generateNormal(x, z, heights, width, depth);

            vertices[index] = Necrosis::Vertex(
                pos,
                // {
                //     (static_cast<f32>(x) - static_cast<f32>(width) / 2.f),
                //     y,
                //     (static_cast<f32>(z) - static_cast<f32>(depth) / 2.f)
                // },
                normal,
                {
                    textureScale * static_cast<f32>(x)/static_cast<f32>(width),
                    textureScale * static_cast<f32>(z)/static_cast<f32>(depth)
                },
                {
                    1.f,
                    x >= width - 1 ? 0.f : heights[z * width + x + 1],
                    0.f
                }
            );

            // normalLines.emplace_back(Necrosis::Vertex(vertices[index].position, {}, {}));
            // normalLines.emplace_back(Necrosis::Vertex(vertices[index].position + normal, {}, {}));

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

    // std::vector<glm::vec3> normals(width * depth);
    // std::vector<glm::vec3> fastNormals(width * depth);
    //
    // slog::info("Started measuring normal computation for terrain of size {}x{}", width, depth);
    // auto start = std::chrono::high_resolution_clock::now();
    //
    // for (u32 z = 0; z < depth; z++) {
    //     for (u32 x = 0; x < width; x++) {
    //         normals.emplace_back(generateNormal(x, z, heights, width, depth));
    //     }
    // }
    // auto time1 = std::chrono::high_resolution_clock::now();
    // for (u32 z = 0; z < depth; z++) {
    //     for (u32 x = 0; x < width; x++) {
    //         fastNormals.emplace_back(generateNormalFast(x, z, heights, width, depth));
    //     }
    // }
    // auto time2 = std::chrono::high_resolution_clock::now();
    //
    // auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(time1 - start);
    // auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
    //
    // slog::info("Normal time: {}ms", duration1.count());
    // slog::info("Fast Normal time: {}ms", duration2.count());
    //
    // float totalAngleError = 0.f;
    // for (int i = 0; i < normals.size(); i++) {
    //     // auto cosTheta = glm::dot(normals[i], fastNormals[i]);
    //     auto cosTheta = glm::clamp(glm::dot(normals[i], fastNormals[i]), -1.0f, 1.0f);
    //     auto angle = glm::acos(cosTheta);
    //
    //     totalAngleError += angle;
    // }

    // slog::info("The accumulated similarity is {:.3f}", totalAngleError / (normals.size()));

    // send data to the GPU
    _vao->bind();
    _vbo->setData(vertices.data(), vertices.size() * sizeof(Necrosis::Vertex));
    _ibo->setData(indices.data(), indices.size());
    _vao->unbind();

    // _normalVao->bind();
    // _normalVbo->setData(normalLines.data(), normalLines.size() * sizeof(Necrosis::Vertex));
    // _normalVao->unbind();
}

}
