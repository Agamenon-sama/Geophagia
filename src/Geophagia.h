#pragma once

#include <memory>

#include <Necrosis/Engine.h>
#include <Necrosis/renderer/Renderer.h>
#include <Necrosis/InputManager.h>
#include <Necrosis/scene/Camera.h>
#include <Necrosis/renderer/Texture.h>
#include <Necrosis/renderer/FrameBuffer.h>

#include "Terrain/Terrain.h"

namespace Geophagia {

class Geophagia : public Necrosis::Engine {
public:
    Geophagia();
    ~Geophagia() = default;

    void run();

private:
    std::unique_ptr<Necrosis::Renderer> _renderer;
    std::unique_ptr<Necrosis::EventManager> _eventManager;
    Necrosis::Camera _camera;
    Necrosis::InputManager _input;
    std::shared_ptr<Necrosis::Shader> shader = nullptr;
    Necrosis::TextureID _texture;
    std::unique_ptr<Necrosis::Framebuffer> _framebuffer;

    Terrain _terrain;
};

}
