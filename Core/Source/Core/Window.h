#pragma once

#include "Event.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <functional>

namespace Core {

    struct WindowParams {
        std::string Title;
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool IsResizible = true;
        bool VSync = true;

        using EventCallbackFn = std::function<void(Event&)>;
        EventCallbackFn EventCallback;
    };

    class Window 
    {
    public:
        Window(const WindowParams& params = WindowParams());
        ~Window();

        void Create();
        void Destroy();
        void Update();

        void RaiseEvent(Event& event);

        GLFWwindow* GetHandle();

        glm::vec2 GetCursorPos();
        glm::vec2 GetFrameBufferSize();

        bool ShouldClose() const;
    private:
        WindowParams m_Params;
        GLFWwindow* m_Handle = nullptr;
    };

}