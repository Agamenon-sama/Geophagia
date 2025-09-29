#pragma once

#include <memory>

#include <Necrosis/Engine.h>
#include <Necrosis/renderer/Renderer.h>
#include <Necrosis/InputManager.h>
#include <Necrosis/scene/Camera.h>

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
};

}