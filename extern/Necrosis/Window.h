#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <functional>

namespace Necrosis {

struct FileFilter {
    std::string name;
    std::string pattern;
};

class Window {
public:
    static void showInfoMessageBox(const std::string &message, const std::string &title = "Information");
    static void showWarningMessageBox(const std::string &message, const std::string &title = "Warning");
    static void showErrorMessageBox(const std::string &message, const std::string &title = "Error");
    static void openFileDialog(
        std::function<void(std::string)> callback,
        const std::vector<FileFilter> &filters = std::vector<FileFilter>(0),
        const char *defaultLocation = nullptr
    );
    static void saveFileDialog(
        std::function<void(std::string)> callback,
        const std::vector<FileFilter> &filters = std::vector<FileFilter>(0),
        const char *defaultLocation = nullptr
    );


    [[nodiscard]]
    static Window* create(std::string title, int width, int height, bool resizable = true);

    Window() = default;
    [[deprecated]]
    Window(std::string title, int width, int height, bool resizable = false);
    ~Window();

    void handleEvents(SDL_Event &ev);
    void free();
    // void createGLContext();
    void swapBuffers();
    void centerMouse();


    int getWidth();
    int getHeight();
    SDL_Window *getSDLWindow() const;
    SDL_GLContext getGLContext() const;
    bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();

private:
    SDL_Window *_window;
    SDL_GLContext _glContext;

    int _width;
    int _height;

    bool _mouseFocus;
    bool _keyboardFocus;
    bool _fullscreen;
    bool _minimized;
    std::string _title;
};

}
