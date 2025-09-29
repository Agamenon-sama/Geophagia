#pragma once

#include <functional>
#include <queue>
#include <vector>
#include <mutex>

#include <slog/slog.h>

namespace Necrosis {

class InputManager;

struct Event {};

template<typename T>
class EventDispatcher {
    // using Listener = void (*)(T);
    using Listener = std::function<void(T)>;
public:
    int listen(Listener l) {
        std::lock_guard<std::mutex> lock(_listenerMtx);

        int handle = _listeners.size();
        _listeners.emplace_back(l);
        return handle;
    }

    // FIXME
    void removeListener(int handle) {
        std::lock_guard<std::mutex> lock(_listenerMtx);

        slog::info("handle: {}", handle);

        slog::info("before deletion: {}", _listeners.size());
        if (!_listeners.empty())
            _listeners.erase(_listeners.begin() + handle);
        slog::info("after deletion: {}", _listeners.size());
    }

    void dispatch(T event) {
        std::lock_guard<std::mutex> lock(_listenerMtx);

        for (const auto &l : _listeners) {
            l(event);
        }
    }

    void enqueue(T event) {
        std::lock_guard<std::mutex> lock(_queueMtx);

        _eventQueue.push(event);
    }

    void dispatchQueue() {
        std::lock_guard<std::mutex> lock(_queueMtx);

        while (!_eventQueue.empty()) {
            auto event = _eventQueue.front();
            _eventQueue.pop();
            // should I lock _listenersMtx ?
            std::lock_guard<std::mutex> listenerLock(_listenerMtx);
            for (const auto &l : _listeners) {
                l(event);
            }
        }
    }

private:
    // TODO: use some associative data structure instead of vector
    std::vector<Listener> _listeners;
    std::mutex _listenerMtx;

    std::queue<T> _eventQueue;
    std::mutex _queueMtx;
};

/**
 * @brief Manages all the events
 *
 * This class reads all the events from SDL and dispatches them to the appropriate
 * input manager (keyboard, mouse, joystick, etc...)
*/
class EventManager {
public:
    EventManager(InputManager *input);

    void manageEvents();
public:
    bool appIsRunning;

private:
    InputManager *_inputManager;
};

}
