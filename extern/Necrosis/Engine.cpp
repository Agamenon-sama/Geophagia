#include "Engine.h"

#define SLOG_IMPLEMENTATION
#include <slog/slog.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Necrosis {

// Engine Engine::instance;

void APIENTRY openglDebugMessageHandler(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei, const GLchar* message, const void*) {
    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string msg = "---------------\n";
    msg += std::format("Debug message ({}): {}\n", id, message);

    switch (source) {
        case GL_DEBUG_SOURCE_API:             msg += "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   msg += "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: msg += "Source: Shader Compiler"; break;
            // ... other sources
    }
    msg += "\n";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               msg += "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msg += "Type: Deprecated Behavior"; break;
            // ... other types
    }
    msg += "\n";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         msg += "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       msg += "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          msg += "Severity: low"; break;
            // ... other severities
    }

    slog::error(msg);
}


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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG); // better debugging but slower
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

    // should give us less trouble with textures
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // enable this for better debugging and should be commented it in release
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglDebugMessageHandler, 0);

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

Engine::~Engine() {

    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

    delete _mainWindow;
    SDL_Quit();
}

void Engine::startGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
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
