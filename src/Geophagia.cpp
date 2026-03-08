#include "Geophagia.h"
#include "Necrosis/Window.h"

#include <SDL3/SDL_events.h>
#include <memory>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <imgui/imgui.h>

#include <Necrosis/renderer/Shader.h>
#include <Necrosis/scene/Mesh.h>

namespace Geophagia {

void Geophagia::uiRender() {
    ImGui::Begin("Renderer");

        ImGui::Text("last frame: %.3f ms, fps: %.3f", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::Text("OpenGL info:");
        ImGui::BulletText("vendor: %s", (const char*)glGetString(GL_VENDOR));
        ImGui::BulletText("version: %s", (const char*)glGetString(GL_VERSION));
        ImGui::BulletText("renderer: %s", (const char*)glGetString(GL_RENDERER));
        ImGui::BulletText("anisotropy samples supported: %.0fx", Necrosis::TextureSampler::getMaxAnisotropySamples());

        ImGui::Separator();
        ImGui::Text("Light:");

        ImGui::SliderFloat("X position", &_lightPosition[0], -0.5f, 0.5f);
        ImGui::SliderFloat("Y position", &_lightPosition[1], 0.f, 1.f);
        ImGui::SliderFloat("Z position", &_lightPosition[2], -0.5f, 0.5f);

    ImGui::End();
}

void Geophagia::renderDockSpace() {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("DockSpace", nullptr, windowFlags);
        const ImGuiID dockSpace =  ImGui::GetID("MasterDockSpace");
        ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f)/*, ImGuiDockNodeFlags_PassthruCentralNode*/);
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) { Necrosis::Window::showWarningMessageBox("This feature is not implemented yet"); }
                if (ImGui::MenuItem("Open")) { Necrosis::Window::showWarningMessageBox("This feature is not implemented yet"); }
                if (ImGui::MenuItem("Save")) { Necrosis::Window::showWarningMessageBox("This feature is not implemented yet"); }
                if (ImGui::BeginMenu("Save As..")) {
                    if (ImGui::MenuItem("PNG")) {
                        Necrosis::Window::saveFileDialog([this](std::string path) {
                            if (path == "") { return; }
                            if (!_terrain.saveAsPng(path)) {
                                std::string msg = std::format("Failed to write to file '{}'", path);
                                slog::warning(msg);
                                Necrosis::Window::showWarningMessageBox(msg);
                            }
                        }, {{"PNG Image", ".png"}});
                    }
                    if (ImGui::MenuItem("Raw Heightmap")) {
                        Necrosis::Window::saveFileDialog([this](std::string path) {
                            if (path == "") { return; }
                            if (!_terrain.saveAsRaw(path)) {
                                std::string msg = std::format("Failed to write to file '{}'", path);
                                slog::warning(msg);
                                Necrosis::Window::showWarningMessageBox(msg);
                            }
                        }, {{"Raw Heightmap", ".raw"}});
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Export")) { Necrosis::Window::showWarningMessageBox("This feature is not implemented yet"); }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    SDL_Event ev = { .type = SDL_EVENT_QUIT };
                    SDL_PushEvent(&ev);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Nothing here yet")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

    ImGui::End();
}

Geophagia::Geophagia()
        : Necrosis::Engine({ .windowTitle = "Geophagia", .windowWidth = 1600, .windowHeight = 900 })
        , _camera(glm::vec3(10.f, 50.f, 25.f)), _isFramebufferHovered(false), _lightPosition(0.8f, 0.8f, 1.f) {

    _camera.movementSpeed = 0.05f;
    _camera.near = 1.f;
    _camera.far = 1000.f;
    _eventManager = std::make_unique<Necrosis::EventManager>(&_input);
    _renderer = std::make_unique<Necrosis::Renderer>();
    _renderer->setCamera(&_camera);

    _setupMouseEventListeners();
    _input.keyboard.keyDispatcher.listen([this](Necrosis::KeyboardEvent ev) {
        static bool mode = false;
        // I'm using 'Z' for 'W' be azerty keyboard. This is temporary
        if (ev.state == Necrosis::KeyState::Up && ev.key == SDL_SCANCODE_Z) {
            _renderer->setWireframeMode(mode = !mode);
        }
    });

    auto texture = Necrosis::TextureManager::makeTextureFromFile("../res/textures/Soil_1.jpg");
    slog::info("Texture loaded: {}x{}",
        Necrosis::TextureManager::getTextureFromID(texture).getWidth(),
        Necrosis::TextureManager::getTextureFromID(texture).getHeight()
    );

    _terrainShader = Necrosis::Shader::makeFromFile("../res/shaders/terrain.glsl");
    if (!_terrainShader) {
        slog::error("Failed to load terrain shader");
        exit(1);
    }
    _terrainShader->setInt("tex", 0);

    _shadowMapShader = Necrosis::Shader::makeFromFile("../res/shaders/ShadowMapping.glsl");
    if (!_shadowMapShader) {
        slog::error("Failed to load the shadow map shader");
        exit(1);
    }

    _framebuffer = std::make_unique<Necrosis::Framebuffer>(glm::ivec4(0, 0, 1280, 720));
    _shadowMapFramebuffer = std::make_unique<Necrosis::Framebuffer>(
        glm::ivec4(0, 0, 1024, 1024),
        Necrosis::FramebufferType::Depth
    );

    // if (!_terrain.loadRawFromFile("../res/heightmap.raw")) {
    if (!_terrain.loadImageFromFile("../res/Heightmap.png")) {
        exit(1);
    }

    _terrain.setTexture(texture);

    _voronoiGenerator = std::make_unique<VoronoiGenerator>(&_terrain);
    _fractalGenerator = std::make_unique<FractalGenerator>(&_terrain);
    _erosionGenerator = std::make_unique<ErosionGenerator>(&_terrain);
}

void Geophagia::run() {
    while (_eventManager->appIsRunning) {
        _eventManager->manageEvents();

        startGuiFrame();

        _shadowMapPass();

        _terrainPass();

        _guiPass();

        endGuiFrame();

        swapBuffers();
    }
    Necrosis::TextureManager::destroyAll();
}

void Geophagia::_setupMouseEventListeners() {
    _input.mouse.motionDispatcher.listen([this](Necrosis::MouseMotionEvent ev) {
        if (!_isFramebufferHovered) return;

        if (_input.mouse.buttons[(int)Necrosis::MouseButton::Middle]) {
            if (_input.keyboard.isPressed(SDL_SCANCODE_LSHIFT)) {
                if (ev.xrel > 0)
                    _camera.processPosition(Necrosis::CameraMovement::Left, 7.f);
                else if (ev.xrel < 0)
                    _camera.processPosition(Necrosis::CameraMovement::Right, 7.f);

                if (ev.yrel > 0)
                    _camera.processPosition(Necrosis::CameraMovement::Up, 7.f);
                else if (ev.yrel < 0)
                    _camera.processPosition(Necrosis::CameraMovement::Down, 7.f);
            }
            else {
                _camera.processAngle(static_cast<f32>(ev.xrel), static_cast<f32>(-ev.yrel));
            }
        }
    });
    _input.mouse.wheelDispatcher.listen([this](Necrosis::ScrollWheelEvent ev) {
        if (!_isFramebufferHovered) return;
        if (ev.scroll > 0.f) {
            const auto originalSpeed = _camera.movementSpeed;
            _camera.movementSpeed = 5.f * originalSpeed;
            _camera.processPosition(Necrosis::CameraMovement::Forward, 7.f);
            _camera.movementSpeed = originalSpeed;
        }
        else {
            const auto originalSpeed = _camera.movementSpeed;
            _camera.movementSpeed = 5.f * originalSpeed;
            _camera.processPosition(Necrosis::CameraMovement::Back, 7.f);
            _camera.movementSpeed = originalSpeed;
        }
    });
}

void Geophagia::_shadowMapPass() {
    _shadowMapFramebuffer->bind();
    _renderer->clear();

    glm::mat4 proj = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -1.0f, 1000.0f);
    glm::mat4 view = glm::lookAt(
        _lightPosition * glm::vec3(_terrain.getWidth(), 255.f, _terrain.getDepth()),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    _shadowMapShader->use();
    _shadowMapShader->setMat4f("u_projection", proj);
    _shadowMapShader->setMat4f("u_view", view);

    _terrain.render();

    _shadowMapFramebuffer->unbind();
}


void Geophagia::_terrainPass() {
    _framebuffer->bind();
    _renderer->clear();

    glm::mat4 lightProj = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -1.0f, 1000.0f);
    glm::mat4 lightView = glm::lookAt(
        _lightPosition * glm::vec3(_terrain.getWidth(), 255.f, _terrain.getDepth()),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    auto lightSpaceMatrix = lightProj * lightView;

    // _renderer->renderAll();
    _terrainShader->use();
    // _terrainShader->setMat4f("u_model", model);
    _terrainShader->setMat4f("u_view", _camera.getViewMatrix());
    _terrainShader->setMat4f("u_projection", _camera.getProjMatrix());
    _terrainShader->setVec3f(
        "u_lightPos",
        _lightPosition * glm::vec3(_terrain.getWidth(), 255.f, _terrain.getDepth())
    );
    _terrainShader->setMat4f("u_lightSpaceMatrix", lightSpaceMatrix);

    auto shadowMapTex = _shadowMapFramebuffer->getTextureID();
    Necrosis::TextureManager::bind(shadowMapTex, 1);
    _terrainShader->setInt("u_shadowMap", 1);

    _terrain.render();
    _framebuffer->unbind();
}

void Geophagia::_guiPass() {
    _renderer->clear();

    renderDockSpace();
    uiRender();


    // ImGui::ShowDemoWindow();
    ImGui::Begin("Image");
    ImGui::Image(
        (ImTextureRef) _framebuffer->getTextureID(),
        ImVec2((f32) _framebuffer->getWidth(), (f32) _framebuffer->getHeight()),
        ImVec2(0, 1), ImVec2(1, 0)
    );
    if (ImGui::IsItemHovered()) { _isFramebufferHovered = true; }
    else { _isFramebufferHovered = false; }
    ImGui::End();
    _terrain.uiDrawHeightmapTexture();
    _terrain.uiRender();

    _voronoiGenerator->uiRender();
    _fractalGenerator->uiRender();
    _erosionGenerator->uiRender();
    _erosionGenerator->update();
}



}
