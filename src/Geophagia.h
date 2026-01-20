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
#include "Terrain/Generators/FbmGenerator.h"

namespace Geophagia {

class Geophagia : public Necrosis::Engine {
public:
    Geophagia();
    ~Geophagia() = default;

    void run();

    void renderDockSpace();

private:
    std::unique_ptr<Necrosis::Renderer> _renderer;
    std::unique_ptr<Necrosis::EventManager> _eventManager;
    Necrosis::Camera _camera;
    Necrosis::InputManager _input;
    bool _isFramebufferHovered;

    std::shared_ptr<Necrosis::Shader> shader = nullptr;
    Necrosis::TextureID _texture;
    Necrosis::TextureSampler _textureSampler;
    std::unique_ptr<Necrosis::Framebuffer> _framebuffer;

    Terrain _terrain;
    std::unique_ptr<VoronoiGenerator> _voronoiGenerator;
    std::unique_ptr<FbmGenerator> _fbmGenerator;

    void _setupMouseEventListeners();
};

}
