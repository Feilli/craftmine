#pragma once

#include "Window.h"
#include "Layer.h"
#include "Event.h"

#include <vector>
#include <string>
#include <memory>

namespace Core {

    struct ApplicationParams {
        std::string Name = "CraftMine";
        WindowParams WindowParams;
    };

    class Application {
    public:
        Application(const ApplicationParams& params);
        ~Application();

        void Run();
        void Stop();

        void RaiseEvent(Event& event);

        template<typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
        void PushLayer() {
            m_LayerStack.push_back(std::make_unique<TLayer>());
        }

        template<typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
        TLayer* GetLayer() {
            for(const auto& layer : m_LayerStack) {
                if(auto casted = dynamic_cast<TLayer*>(layer.get())) {
                    return casted;
                }
            }

            return nullptr;
        }

        std::shared_ptr<Window> GetWindow();

        glm::vec2 GetFrameBufferSize() const;
        int GetTickCount() const;

        static Application& Get();
        static float GetTime();
    private:
        ApplicationParams m_Params;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<EventDispatcher> m_EventDispatcher;

        bool m_Running = false;
        int m_TickCount = 0;

        std::vector<std::unique_ptr<Layer>> m_LayerStack;

        friend class Layer;
    };

}