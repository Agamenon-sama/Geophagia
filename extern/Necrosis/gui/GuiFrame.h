#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#include <SDL3/SDL_events.h>

namespace Necrosis {

class GuiFrame {
public:
    GuiFrame();
    // virtual ~GuiFrame();

    virtual void handleEvents(SDL_Event &event);

    // render should call the 3 functions below
    // this function is meant to be overriden by the child and not called
    // throw this class.
    virtual void render() = 0;

    // these can called individually in this order or together
    // using the render function
    virtual void beginRender();
    // this class doesn't implement this function and it shouldn't be called
    // but child classes should implement it
    virtual void uiRender() = 0;
    virtual void endRender();
};

}
