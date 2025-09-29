#pragma once

#include "Window.h"

namespace Necrosis {

struct EngineInitSetup {
    // Main window options
    std::string windowTitle = "Necrosis";
    int windowWidth = 1280;
    int windowHeight = 720;
};

class Engine {
public:
    Engine(const Engine&) = delete;

    // static void init(EngineInitSetup initSetup = {});
    // static void swapBuffers() { instance._mainWindow->swapBuffers(); } // I hate this
    // static Window* getMainWindow() { return instance._mainWindow; } // I hate this

// private:
    Engine(const EngineInitSetup initSetup = {});
    ~Engine();

    void swapBuffers() { _mainWindow->swapBuffers(); }
    Window* getMainWindow() const { return _mainWindow; }

    void startGuiFrame();
    void endGuiFrame();

protected:
    // static Engine instance;

    // TODO: I must change the window management system
    Window *_mainWindow = nullptr;
};

}
