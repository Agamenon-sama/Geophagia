#include "Window.h"

#include <iostream>
#include <cassert>

#include <glad/glad.h>
#include <slog/slog.h>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

namespace Necrosis {

Window* Window::create(std::string title, int width, int height, bool resizable) {
    Window *res = new Window();

    uint32_t flags = SDL_WINDOW_OPENGL;
    if (resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    
    res->_window = SDL_CreateWindow(title.c_str(), width, height, flags);
    if (!res->_window) {
        slog::error("Failed to create window: {}", SDL_GetError());
        return nullptr;
    }

    res->_glContext = SDL_GL_CreateContext(res->_window);
    if (!res->_glContext) {
        slog::error("Failed to create opengl context: {}", SDL_GetError());
        return nullptr;
    }

    if (!SDL_GL_SetSwapInterval(-1)) {
        slog::warning("Failed to enable adaptive vsync: {}. Trying with vsync.", SDL_GetError());
        if (!SDL_GL_SetSwapInterval(1)) {
            slog::warning("Failed to enable vsync: {}. Will run with immediate updates.", SDL_GetError());
        }
    }

    res->_width = width;
    res->_height = height;
    res->_title = title;
    res->_mouseFocus = true;
    res->_keyboardFocus = true;
    res->_fullscreen = false;
    res->_minimized = true;

    return res;
}

Window::Window(std::string title, int width, int height, bool resizable) {
    uint32_t flags = SDL_WINDOW_OPENGL;
    if (resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    
    _window = SDL_CreateWindow(title.c_str(), width, height, flags);

    if (!_window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << "\nExiting\n";
        exit(EXIT_FAILURE);
    }

    _glContext = SDL_GL_CreateContext(_window);
    if (!_glContext) {
        std::cerr << "Failed to create opengl context: " << SDL_GetError() << "\nExiting\n";
        exit(EXIT_FAILURE);
    }

    _width = width;
    _height = height;
    _title = title;
    _mouseFocus = true;
    _keyboardFocus = true;
    _fullscreen = false;
    _minimized = true;
}

Window::~Window() {
    free();
}

void Window::handleEvents(SDL_Event &ev) {
    // if (ev.type == SDL_EVENT) {
        // switch (ev.window.event) {
    switch (ev.type) { // window events
        // case SDL_WINDOWEVENT_SIZE_CHANGED: // changing window size
        case SDL_EVENT_WINDOW_RESIZED:
            _width = ev.window.data1;
            _height = ev.window.data2;
            glViewport(0, 0, _width, _height);
            swapBuffers(); // Experimental
            break;
        // case SDL_WINDOWEVENT_EXPOSED:
        case SDL_EVENT_WINDOW_EXPOSED:
            swapBuffers(); // Experimental
            break;
        // case SDL_WINDOWEVENT_ENTER: // Mouse on the window
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            _mouseFocus = true;
            break;
        // case SDL_WINDOWEVENT_LEAVE: // Mouse off the window
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            _mouseFocus = false;
            break;
        // case SDL_WINDOWEVENT_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            _keyboardFocus = true;
            break;
        // case SDL_WINDOWEVENT_FOCUS_LOST:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            _keyboardFocus = false;
            break;

        default:
            break;
    }
    /* } else  */if (ev.type == SDL_EVENT_KEY_DOWN && ev.key.key == SDLK_F11) {
        if(_fullscreen) {
            SDL_SetWindowFullscreen(_window, 0);
            _fullscreen = false;
        } else {
            SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
            _fullscreen = true;
            _minimized = false;
        }
    }
}

void Window::free() {
    SDL_GL_DestroyContext(_glContext);
    SDL_DestroyWindow(_window);
    _width = 0;
    _height = 0;
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(_window);
}

// void Window::createGLContext() {
//     _glContext = SDL_GL_CreateContext(_window);
// }
void Window::showInfoMessageBox(const std::string &message, const std::string &title) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), nullptr);
}
void Window::showWarningMessageBox(const std::string &message, const std::string &title) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title.c_str(), message.c_str(), nullptr);
}
void Window::showErrorMessageBox(const std::string &message, const std::string &title) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);
}

void Window::openFileDialog(
        std::function<void(std::string)> callback,
        const std::vector<FileFilter> &filters, const char *defaultLocation
    ) {
    // we allocate the vector on the heap to insure it's alive until the callback sees it
    // we will deallocate it later. Shut up clangd (`ー´)
    auto sdl_filters = new std::vector<SDL_DialogFileFilter>();
    for (const auto& f : filters) {
        sdl_filters->push_back({f.name.c_str(), f.pattern.c_str()});
    }

    struct Context {
        std::function<void(std::string)> userCallback;
        std::vector<SDL_DialogFileFilter>* internalFilters;
    };

    // another heap allocation for the same reason
    auto ctx = new Context{callback, sdl_filters};

    SDL_ShowOpenFileDialog([](void *userdata, const char * const *filelist, int) {
        assert(userdata && "WTF SDL!! userdata is null");
        auto *ctx = static_cast<Context*>(userdata);

        if (filelist && *filelist) {
            ctx->userCallback(std::string(*filelist));
        }
        else {
            ctx->userCallback("");
            if (!filelist) {
                slog::warning("Unexpected error getting filename: {}", SDL_GetError());
            }
        }

        // now we can delete the heap allocated stuff
        delete ctx->internalFilters;
        delete ctx;
    }, ctx, nullptr, sdl_filters->data(), (int)sdl_filters->size(), defaultLocation, false);
}

void Window::saveFileDialog(
        std::function<void(std::string)> callback,
        const std::vector<FileFilter> &filters, const char *defaultLocation
    ) {
    auto sdl_filters = new std::vector<SDL_DialogFileFilter>();
    for (const auto& f : filters) {
        sdl_filters->push_back({f.name.c_str(), f.pattern.c_str()});
    }

    struct Context {
        std::function<void(std::string)> userCallback;
        std::vector<SDL_DialogFileFilter>* internalFilters;
    };

    auto ctx = new Context{callback, sdl_filters};

    SDL_ShowSaveFileDialog([](void *userdata, const char * const *filelist, int) {
        assert(userdata && "WTF SDL!! userdata is null");
        auto *ctx = static_cast<Context*>(userdata);

        if (filelist && *filelist) {
            ctx->userCallback(std::string(*filelist));
        }
        else {
            ctx->userCallback("");
            if (!filelist) {
                slog::warning("Unexpected error getting filename: {}", SDL_GetError());
            }
        }

        delete ctx->internalFilters;
        delete ctx;
    }, ctx, nullptr, sdl_filters->data(), (int)sdl_filters->size(), defaultLocation);
}

void Window::centerMouse() {
    SDL_WarpMouseInWindow(_window, _width / 2.f, _height / 2.f);
}


int Window::getWidth() { return _width; }
int Window::getHeight() { return _height; }
SDL_Window* Window::getSDLWindow() const { return _window; }
SDL_GLContext Window::getGLContext() const { return _glContext; }
bool Window::hasMouseFocus() { return _mouseFocus; }
bool Window::hasKeyboardFocus() { return _keyboardFocus; }
bool Window::isMinimized() { return _minimized; }

// void Window::setFullscreenMode(uint32_t mode) {
//     if(mode == SDL_WINDOW_FULLSCREEN || mode == SDL_WINDOW_FULLSCREEN_DESKTOP) {
//         _fullscreenMode = mode;
//     }
// }

}
