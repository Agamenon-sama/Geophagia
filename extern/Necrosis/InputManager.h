#pragma once

#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_mouse.h>

#include "Common.h"
#include "Events.h"

namespace Necrosis {

enum class KeyState : u8 {
    Unknown,
    Down, ///< @brief Key is pressed down
    Repeat, ///< @brief Key is pressed down and has been held down at least since last frame
    Up, ///< @brief Key is pressed up
    Idle ///< @brief Key is pressed up and has been held up at least since last frame
};

struct KeyboardEvent : public Event {
    SDL_Scancode key;
    KeyState state;
};

/**
 * @brief Keyboard handling class
 *
 * This class presents an interface to look at the state of the keyboard
 * and to dispatch keyboard events.
*/
class Keyboard {
public:
    Keyboard();
    ~Keyboard();

    void update();
    bool isPressed(SDL_Scancode k) const;

public:
    KeyState *keys;

    EventDispatcher<KeyboardEvent> keyDispatcher;
};

#pragma region MouseRegion

enum class MouseButton : u8 {
    Unknown,
    Left,
    Middle, ///< @brief Mouse wheel button
    Right,
    X1, ///< @brief Extra button 1, usually the back button
    X2 ///< @brief Extra button 2, usually the forward button
};

enum class MouseButtonEventType : u8 {
    Unknown,
    Down,
    Up
};

struct MouseMotionEvent : public Event {
    int x;
    int y;
    int xrel;
    int yrel;
};

struct MouseButtonEvent : public Event {
    MouseButton button;
    MouseButtonEventType type;
};

struct ScrollWheelEvent : public Event {
    float scroll;
};

/**
 * @brief Mouse handling class
 *
 * This class presents an interface to look at the state of the mouse
 * and to dispatch mouse events.
*/
class Mouse {
public:
    Mouse();

    void setRelativeMode(bool on);

public:
    bool buttons[5];
    
    int x;
    int y;
    int xrel;
    int yrel;

    EventDispatcher<MouseMotionEvent> motionDispatcher;
    EventDispatcher<MouseButtonEvent> buttonDispatcher;
    EventDispatcher<ScrollWheelEvent> wheelDispatcher;
};

#pragma endregion MouseRegion

/**
 * @brief Input manager class
 *
 * This class contains instances of all input devices.
*/
class InputManager {
public:
    Keyboard keyboard;
    Mouse mouse;
    // TODO: Add support for game controllers
};

}
