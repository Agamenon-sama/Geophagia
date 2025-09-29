#include "Mesh.h"

#include <assert.h>

#include <glad/glad.h>

namespace Necrosis {

Mesh Mesh::makeQuad() {
    std::vector<Vertex> verts;

    verts.push_back({ // top right
        .position = glm::vec3(1.f, 1.f, 0.f),
        .normal = glm::vec3(0.f, 0.f, 1.f),
        .texCoord = glm::vec2(1.f, 1.f)
    });
    verts.push_back({ // top left
        .position = glm::vec3(-1.f, 1.f, 0.f),
        .normal = glm::vec3(0.f, 0.f, 1.f),
        .texCoord = glm::vec2(0.f, 1.f)
    });
    verts.push_back({ // bot left
        .position = glm::vec3(-1.f, -1.f, 0.f),
        .normal = glm::vec3(0.f, 0.f, 1.f),
        .texCoord = glm::vec2(0.f, 0.f)
    });
    /* verts.push_back({ // top right
        .position = glm::vec3(1.f, 1.f, 0.f),
        .normal = glm::vec3(0.f, 0.f, 1.f),
        .texCoord = glm::vec2(1.f, 1.f)
    });
    verts.push_back({ // bot left
        .position = glm::vec3(-1.f, -1.f, 0.f),
        .normal = glm::vec3(0.f, 0.f, 1.f),
        .texCoord = glm::vec2(0.f, 0.f)
    }); */
    verts.push_back({ // bot right
        .position = glm::vec3(1.f, -1.f, 0.f),
        .normal = glm::vec3(0.f, 0.f, 1.f),
        .texCoord = glm::vec2(1.f, 0.f)
    });

    return Mesh(verts, {0, 1, 2, 0, 2, 3}, {});
}

Mesh Mesh::makeCube() {
    // trust me, it was more painful for me to write than it is for you to read
    std::vector<Vertex> verts;
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});

    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)});

    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});

    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});

    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});

    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
    verts.push_back((Vertex){glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});

    return Mesh(verts, {}, {});
}

Mesh::Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds, std::vector<TextureID> texs)
        : vertices(verts) , indices(inds) , textures(texs) {
            
    _setupBuffers();
}

void Mesh::_setupBuffers() {
    _vao = std::make_shared<VertexArray>();
    _vao->bind();

    _vbo = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(Vertex));
    _vbo->bind();
    if (indices.size()) {
        _ebo = std::make_shared<IndexBuffer>(&indices[0], indices.size());
        _ebo->bind();
    }

    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3); // pos
    layout.push(GL_FLOAT, 3); // norm
    layout.push(GL_FLOAT, 2); // tex
    _vao->addBuffer(*_vbo, layout);
}

void Mesh::render() const {
    _vao->bind();
    _vbo->bind();
    if (_ebo)
        _ebo->bind();
    if (!indices.size())
        glDrawArrays(GL_TRIANGLES, 0, _vbo->count());
    else
        glDrawElements(GL_TRIANGLES, _ebo->getCount(), GL_UNSIGNED_INT, 0);
}

}
