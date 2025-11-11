#pragma once

#include "Window.h"
#include "Layer.h"
#include "event.h"

#include <vector>
#include <string>
#include <memory>

namespace Core {

    struct ApplicationParams {
        std::string Name = "MineCraft";
        WindowParams WindowParams;
    };

    class Application {
    public:
        Application(const ApplicationParams& params = ApplicationParams());
        ~Application();

        void Run();
        void Stop();

        template<typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
        void PushLayer() {
            m_LayerStack.push_back(std::make_unique<TLayer>());
        }

        std::shared_ptr<Window> GetWindow();
        std::shared_ptr<EventDispatcher> GetEventDispatcher();

        glm::vec2 GetCursorPos() const;
        glm::vec2 GetFrameBufferSize() const;

        static Application& Get();
        static float GetTime();

        static void OnSetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnSetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    private:
        ApplicationParams m_Params;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<EventDispatcher> m_EventDispatcher;

        bool m_Running = false;

        std::vector<std::unique_ptr<Layer>> m_LayerStack;
    };

}