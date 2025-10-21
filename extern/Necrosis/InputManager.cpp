#include "InputManager.h"

#include <SDL3/SDL_keyboard.h>

#include "Common.h"

static const u16 maxKeysNumber = 300; // SDL_NUM_SCANCODES == 512 but 300 is enough

namespace Necrosis {

Keyboard::Keyboard() {
    keys = new KeyState[maxKeysNumber];
    for (int i = 0; i < maxKeysNumber; i++) {
        keys[i] = KeyState::Idle;
    }
}
Keyboard::~Keyboard() {
    delete [] keys;
}

void Keyboard::update() {
    auto sdlkeys = SDL_GetKeyboardState(nullptr);

    for (int i = 0; i < maxKeysNumber; i++) {
        if (sdlkeys[i]) {
            if (keys[i] == KeyState::Idle) {
                keys[i] = KeyState::Down;

                KeyboardEvent ev = {
                    .key = (SDL_Scancode) i,
                    .state = KeyState::Down
                };
                keyDispatcher.enqueue(ev);
            }
            else if (keys[i] == KeyState::Down) {
                keys[i] = KeyState::Repeat;
            }
        }
        else {
            if (isPressed((SDL_Scancode) i)) {
                keys[i] = KeyState::Up;

                KeyboardEvent ev = {
                    .key = (SDL_Scancode) i,
                    .state = KeyState::Up
                };
                keyDispatcher.enqueue(ev);
            }
            else if (keys[i] == KeyState::Up) {
                keys[i] = KeyState::Idle;
            }
        }
    }

    keyDispatcher.dispatchQueue();
}

bool Keyboard::isPressed(SDL_Scancode k) const {
    return keys[k] == KeyState::Down || keys[k] == KeyState::Repeat;
}

Mouse::Mouse() : x(0), y(0), xrel(0), yrel(0), buttons{false, false, false, false, false, false} {}

void Mouse::setRelativeMode(bool enable) { // TODO: adapt for SDL3
    // SDL_SetRelativeMouseMode((SDL_bool) enable);
}

}
