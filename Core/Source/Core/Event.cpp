#include "Event.h"

namespace Core {

    EventDispatcher::EventDispatcher() {

    }

    EventDispatcher::~EventDispatcher() {

    }

    void EventDispatcher::PushEvent(const Event& event) {
        m_EventQueue.push(event);
    }

    void EventDispatcher::AddListener(const Listener& listener) {
        m_Listeners.push_back(listener);
    }

    void EventDispatcher::Dispatch() {
        while(!m_EventQueue.empty()) {
            const Event& event = m_EventQueue.front();
            
            for(const Listener& callback : m_Listeners) {
                callback(event);
            }

            m_EventQueue.pop();
        }
    }

}