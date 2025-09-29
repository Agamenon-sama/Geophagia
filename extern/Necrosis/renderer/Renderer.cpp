#include "Renderer.h"
#include "slog/slog.h"

#include <glad/glad.h>

namespace Necrosis {

Renderer::Renderer() : _camera(nullptr) {
    // Load OpenGL
    // if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
    //     slog::error("Failed to load OpenGL");
    //     exit(EXIT_FAILURE);
    // }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    // This config of stencil testing caused issues with the skybox where
    // the skybox was always drawn on top. Be careful next time you use it
    // glEnable(GL_STENCIL_TEST);
    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_BLEND);
    // blend formula of SDL_BLENDMODE_BLEND as implemented in SDL2
    // <https://wiki.libsdl.org/SDL2/SDL_BlendMode>
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // Additive blending
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
}

Renderer::~Renderer() {}

void Renderer::render(const VertexArray& va, const IndexBuffer &ib, const Shader &shader) const noexcept {
    shader.use();
    va.bind();
    ib.bind();  
    glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
}

void Renderer::render(const VertexArray& va, const VertexBuffer &vb, const Shader &shader) const noexcept {
    shader.use();
    va.bind();
    vb.bind();
    glDrawArrays(GL_TRIANGLES, 0, vb.count()); // / 8);
}

void Renderer::renderAll() const noexcept {
#if !NDEBUG
    // warn the programmer if he forgot to set the camera
    static bool printedThisWarningBefore = false;
    if (!printedThisWarningBefore && !_camera) {
        slog::warning("No camera is set for the Renderer");
        printedThisWarningBefore = true;
    }
    // why do I even bother ? no one is gonna use this anyway
#endif

    for (auto it = _renderCommands.begin(); it != _renderCommands.end(); it++) {
        it->shader->use();
        if (_camera) {
            it->shader->setMat4f("u_view", _camera->getViewMatrix());
            it->shader->setMat4f("u_projection", _camera->getProjMatrix());
        }

        it->renderable->render();
    }

    if (_skybox) {
        _skybox->render();
    }
}

void Renderer::clear() const noexcept {
    glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
    // glClearColor(0.f, 0.f, 0.f, 1.0f);
    // glClearColor(1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::setWireframeMode(bool on) const {
    if (on) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::addRenderCommand(RenderCommand command) {
    _renderCommands.push_back(command);
}

}
