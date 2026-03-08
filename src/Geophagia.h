#pragma once

#include <memory>

#include <Necrosis/Engine.h>
#include <Necrosis/renderer/Renderer.h>
#include <Necrosis/InputManager.h>
#include <Necrosis/scene/Camera.h>
#include <Necrosis/renderer/Texture.h>
#include <Necrosis/renderer/FrameBuffer.h>

#include "Terrain/Terrain.h"
#include "Terrain/Generators/VoronoiGenerator.h"
#include "Terrain/Generators/FractalGenerator.h"
#include "Terrain/Generators/ErosionGenerator.h"

namespace Geophagia {

class Geophagia : public Necrosis::Engine {
public:
    Geophagia();
    ~Geophagia() = default;

    void run();

    void renderDockSpace();
    void uiRender();

private:
    std::unique_ptr<Necrosis::Renderer> _renderer;
    std::unique_ptr<Necrosis::EventManager> _eventManager;
    Necrosis::Camera _camera;
    Necrosis::InputManager _input;
    std::shared_ptr<Necrosis::Shader> _terrainShader;
    std::shared_ptr<Necrosis::Shader> _shadowMapShader;
    bool _isFramebufferHovered;
    glm::vec3 _lightPosition;

    std::unique_ptr<Necrosis::Framebuffer> _framebuffer;
    std::unique_ptr<Necrosis::Framebuffer> _shadowMapFramebuffer;

    Terrain _terrain;
    std::unique_ptr<VoronoiGenerator> _voronoiGenerator;
    std::unique_ptr<FractalGenerator> _fractalGenerator;
    std::unique_ptr<ErosionGenerator> _erosionGenerator;

    void _setupMouseEventListeners();
    void _terrainPass();
    void _guiPass();
    void _shadowMapPass();
};

}
