#include "Events.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <imgui/imgui_impl_sdl3.h>

#include "InputManager.h"
#include "slog/slog.h"

namespace Necrosis {

EventManager::EventManager(InputManager *input) : appIsRunning(true), _inputManager(input) {}

void EventManager::manageEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            appIsRunning = false;
            break;
        }
        else if (event.type == SDL_EVENT_DROP_FILE) {
            // TODO
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            if (_inputManager) {
                _inputManager->mouse.x = event.motion.x;
                _inputManager->mouse.y = event.motion.y;
                _inputManager->mouse.xrel = event.motion.xrel;
                _inputManager->mouse.yrel = event.motion.yrel;

                MouseMotionEvent ev{};
                ev.x = event.motion.x;
                ev.y = event.motion.y;
                ev.xrel = event.motion.xrel;
                ev.yrel = event.motion.yrel;
                _inputManager->mouse.motionDispatcher.enqueue(ev);
            }
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            if (_inputManager) {
                MouseButtonEvent ev{};
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    ev.button = MouseButton::Left;
                    ev.type = MouseButtonEventType::Down;
                    break;
                case SDL_BUTTON_MIDDLE:
                    ev.button = MouseButton::Middle;
                    ev.type = MouseButtonEventType::Down;
                    break;
                case SDL_BUTTON_RIGHT:
                    ev.button = MouseButton::Right;
                    ev.type = MouseButtonEventType::Down;
                    break;
                case SDL_BUTTON_X1:
                    ev.button = MouseButton::X1;
                    ev.type = MouseButtonEventType::Down;
                    break;
                case SDL_BUTTON_X2:
                    ev.button = MouseButton::X2;
                    ev.type = MouseButtonEventType::Down;
                    break;
                default:
                    slog::warning("Unknown mouse event captured");
                    break;
                }
                _inputManager->mouse.buttonDispatcher.enqueue(ev);
            }
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            MouseButtonEvent ev{};
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                ev.button = MouseButton::Left;
                ev.type = MouseButtonEventType::Up;
                break;
            case SDL_BUTTON_MIDDLE:
                ev.button = MouseButton::Middle;
                ev.type = MouseButtonEventType::Up;
                break;
            case SDL_BUTTON_RIGHT:
                ev.button = MouseButton::Right;
                ev.type = MouseButtonEventType::Up;
                break;
            case SDL_BUTTON_X1:
                ev.button = MouseButton::X1;
                ev.type = MouseButtonEventType::Up;
                break;
            case SDL_BUTTON_X2:
                ev.button = MouseButton::X2;
                ev.type = MouseButtonEventType::Up;
                break;
            default:
                slog::warning("Unknown mouse event captured");
                break;
            }
            if (_inputManager) {
                _inputManager->mouse.buttonDispatcher.enqueue(ev);
            }
        }
        else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            const ScrollWheelEvent ev = { .scroll = event.wheel.y };
            if (_inputManager) {
                _inputManager->mouse.wheelDispatcher.enqueue(ev);
            }
        }

        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    if (_inputManager) {
        _inputManager->keyboard.update();

        // handle all queued events in the dispatchers
        _inputManager->mouse.motionDispatcher.dispatchQueue();
        _inputManager->mouse.buttonDispatcher.dispatchQueue();
        _inputManager->mouse.wheelDispatcher.dispatchQueue();
    }
}

}

