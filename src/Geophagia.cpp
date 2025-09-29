#include "Geophagia.h"
#include "Necrosis/Engine.h"
#include "imgui.h"

#include <SDL3/SDL_video.h>
#include <memory>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <Necrosis/renderer/Shader.h>
#include <Necrosis/scene/Mesh.h>

namespace Geophagia {

void uiRender() {
    ImGui::Begin("New window");

        ImGui::Text("last frame: %.3f ms, fps: %.3f", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::Text("OpenGL info:");
        ImGui::BulletText("vendor: %s", (const char*)glGetString(GL_VENDOR));
        ImGui::BulletText("version: %s", (const char*)glGetString(GL_VERSION));
        ImGui::BulletText("renderer: %s", (const char*)glGetString(GL_RENDERER));
        ImGui::BulletText("extensions: %s", (const char*)glGetString(GL_EXTENSIONS));
        // ImGui::Separator();

    ImGui::End();
}

Geophagia::Geophagia()
        : Necrosis::Engine({ .windowTitle = "Geophagia", .windowWidth = 1280, .windowHeight = 720 })
        , _camera(glm::vec3(0.f, 0.f, 3.f)) {

    _eventManager = std::make_unique<Necrosis::EventManager>(&_input);
    _renderer = std::make_unique<Necrosis::Renderer>();
    _renderer->setCamera(&_camera);

    auto cube = Necrosis::Mesh::makeCube();
    shader = Necrosis::Shader::makeFromFile("../res/shaders/basic.glsl");
    _renderer->addRenderCommand({
        .renderable = std::make_shared<Necrosis::Mesh>(cube),
        .shader = shader
    });
}

void Geophagia::run() {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->use();
    shader->setMat4f("u_model", model);

    while (_eventManager->appIsRunning) {
        _eventManager->manageEvents();
        
        _renderer->clear();

        model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader->use();
        shader->setMat4f("u_model", model);

        _renderer->renderAll();

        startGuiFrame();
        uiRender();
        endGuiFrame();

        swapBuffers();
    }
}
}
