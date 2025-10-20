#include "Engine.h"
#include <cstdlib>

#define SLOG_IMPLEMENTATION
#include <slog/slog.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Necrosis {

// Engine Engine::instance;

Engine::Engine(const EngineInitSetup initSetup) {
    // Initilize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        slog::error("Failed to initialize SDL: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Set GL hints
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    slog::info("title: {}\tresolution: {}x{}", initSetup.windowTitle, initSetup.windowWidth, initSetup.windowHeight);
    // instance._mainWindow = Window(initSetup.windowTitle, initSetup.windowWidth, initSetup.windowHeight);
    _mainWindow = Window::create(initSetup.windowTitle, initSetup.windowWidth, initSetup.windowHeight);
    if (!_mainWindow) {
        slog::error("Failed to create main window");
        exit(EXIT_FAILURE);
    }

    // Load OpenGL
    if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
        slog::error("Failed to load OpenGL");
        exit(EXIT_FAILURE);
    }

    // init Dear IMGUI
    IMGUI_CHECKVERSION();
    
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplSDL3_InitForOpenGL(_mainWindow->getSDLWindow(), _mainWindow->getGLContext());
    ImGui_ImplOpenGL3_Init("#version 450 core");

    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF("../res/fonts/Ubuntu-L.ttf", 16.0f);
}

/* void Engine::init(EngineInitSetup initSetup) {
    // Initilize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        slog::error("Failed to initialize SDL: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Set GL hints
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    slog::info("title: {}\tresolution: {}x{}", initSetup.windowTitle, initSetup.windowWidth, initSetup.windowHeight);
    // instance._mainWindow = Window(initSetup.windowTitle, initSetup.windowWidth, initSetup.windowHeight);
    // instance._mainWindow = Window::create(initSetup.windowTitle, initSetup.windowWidth, initSetup.windowHeight);

    // Load OpenGL
    if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
        slog::error("Failed to load OpenGL");
        exit(EXIT_FAILURE);
    }

    // init Dear IMGUI
    // IMGUI_CHECKVERSION();
    
    // ImGui::CreateContext();
    // ImGuiIO &io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // ImGui_ImplSDL3_InitForOpenGL(instance._mainWindow->getSDLWindow(), instance._mainWindow->getGLContext());
    // ImGui_ImplOpenGL3_Init("#version 450 core");

    // ImGui::StyleColorsDark();
} */

Engine::~Engine() {
    slog::debug("~Engine");

    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

    delete _mainWindow;
    SDL_Quit();
}

void Engine::startGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplSDL2_NewFrame(_parentWindow->getSDLWindow());
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Engine::endGuiFrame() {
    ImGui::Render();
    // ImGuiIO &io = ImGui::GetIO();
    // glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    // glClearColor(0.11, 0.11, 0.11, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}
