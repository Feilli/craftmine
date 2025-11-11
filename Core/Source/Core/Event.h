#pragma once

#include <queue>
#include <functional>
#include <vector>

namespace Core {

    enum EventType {
        KeyPressed,
        KeyReleased,
        MouseButtonPressed,
        MouseButtonReleased
    };

    struct Event {
        EventType Type = EventType::KeyPressed;
        int Key = -1;
        int Mods = -1;
    };

    class EventDispatcher {
    public:
        using Listener = std::function<void(const Event&)>;

        EventDispatcher();
        ~EventDispatcher();

        void PushEvent(const Event& event);
        void AddListener(const Listener& listener);
        void Dispatch();

    private:
        std::queue<Event> m_EventQueue;
        std::vector<Listener> m_Listeners;
    };

}
