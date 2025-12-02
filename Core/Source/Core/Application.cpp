#include "Application.h"

#include <assert.h>
#include <iostream>
#include <ranges>

namespace Core {

    static Application* s_App = nullptr;

    static void GLFWErrorCallback(int error, const char* description) {
        std::cerr << "[GLFW Error]: " << description << std::endl;
    }

    Application::Application(const ApplicationParams& params) 
        : m_Params(params) {
        s_App = this;

        // init GLFW
        glfwSetErrorCallback(GLFWErrorCallback);
        glfwInit();

        if(m_Params.WindowParams.Title.empty()) {
            m_Params.WindowParams.Title = m_Params.Name;
        }

        m_Params.WindowParams.EventCallback = [this](Event& event) { RaiseEvent(event); };

        m_Window = std::make_shared<Window>(m_Params.WindowParams);
        m_Window->Create();
    }

    Application::~Application() {
        m_Window->Destroy();

        // terminate GLFW
        glfwTerminate();

        s_App = nullptr;
    }

    void Application::Run() {
        m_Running = true;

        float lastFrameTIme = GetTime();

        int tickCount = 0;
        float tickTime = 0;

        while(m_Running) {
            glfwPollEvents();

            if(m_Window->ShouldClose()) {
                Stop();
                break;
            }

            float currentFrameTime = GetTime();
            float deltaTime = glm::clamp(currentFrameTime - lastFrameTIme, 0.001f, 0.1f);
            lastFrameTIme = currentFrameTime;

            // Update layers
            for(const std::unique_ptr<Layer>& layer: m_LayerStack)
                layer->OnUpdate(deltaTime);

            // Render layers
            for(const std::unique_ptr<Layer>& layer: m_LayerStack)
                layer->OnRender();

            m_Window->Update();

            tickCount += 1;
            tickTime += deltaTime;

            if(tickTime > 1.0f) {
                m_TickCount = tickCount;

                tickCount = 0;
                tickTime = 0;
            }
        }
    }

    void Application::Stop() {
        m_Running = false;
    }

    void Application::RaiseEvent(Event& event) {
        for(auto& layer : std::views::reverse(m_LayerStack)) {
            layer->OnEvent(event);

            if(event.Handled) {
                break;
            }
        }
    }

    std::shared_ptr<Window> Application::GetWindow() {
        return m_Window;
    }

    glm::vec2 Application::GetFrameBufferSize() const {
        return m_Window->GetFrameBufferSize();
    }

    int Application::GetTickCount() const {
        return m_TickCount;
    }

    Application& Application::Get() {
        assert(s_App);
        return *s_App;
    }

    float Application::GetTime() {
        return (float)glfwGetTime();
    }
}