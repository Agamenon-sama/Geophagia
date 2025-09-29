#include "Skybox.h"

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <slog/slog.h>

namespace Necrosis {

Skybox::Skybox(const Camera &camera) : _camera(camera) {
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

    // loading images
    stbi_set_flip_vertically_on_load(false);
    int width, height, bppx;
    uint8_t *data;

    // right
    data = stbi_load("../res/skyboxes/default/right.jpg", &width, &height, &bppx, 3);
    if (!data) {
        slog::error("Failed to load right cubemap image");
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    
    // left
    data = stbi_load("../res/skyboxes/default/left.jpg", &width, &height, &bppx, 3);
    if (!data) {
        slog::error("Failed to load left cubemap image");
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // top
    data = stbi_load("../res/skyboxes/default/top.jpg", &width, &height, &bppx, 3);
    if (!data) {
        slog::error("Failed to load top cubemap image");
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // bottom
    data = stbi_load("../res/skyboxes/default/bottom.jpg", &width, &height, &bppx, 3);
    if (!data) {
        slog::error("Failed to load bottom cubemap image");
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // back
    data = stbi_load("../res/skyboxes/default/back.jpg", &width, &height, &bppx, 3);
    if (!data) {
        slog::error("Failed to load back cubemap image");
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // front
    data = stbi_load("../res/skyboxes/default/front.jpg", &width, &height, &bppx, 3);
    if (!data) {
        slog::error("Failed to load front cubemap image");
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    _vao = new VertexArray();
    _vbo = new VertexBuffer(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    _vao->addBuffer(*_vbo, layout);

    _shader = Shader::makeFromFile("../res/shaders/skybox.glsl");
    if (!_shader) {
        slog::error("Failed to create skybox shader");
    }
}

Skybox::~Skybox() {
    glDeleteTextures(1, &_texture);

    delete _vbo;
    delete _vao;
}

void Skybox::render() const {
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);

    // glDepthRange(1.f, 1.f);

    _shader->use();
    _shader->setMat4f("u_projection", _camera.getProjMatrix());
    _shader->setMat4f("u_view", _camera.getViewMatrix());

    glm::mat4 shift = glm::mat4(1.f);
    shift = glm::translate(shift, _camera.position);
    _shader->setMat4f("u_translate", shift);

    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
    // renderer.render(*_vao, *_vbo, *_shader);
    _vao->bind();
    _vbo->bind();

    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    // glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    // glDepthRange(0, 1.f);
}

}
