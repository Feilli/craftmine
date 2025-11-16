#include "Application.h"

#include <glm/glm.hpp>

#include <assert.h>
#include <print>

namespace Core {

    static Application* s_App = nullptr;

    Application::Application(const ApplicationParams& params) 
        : m_Params(params) {
        s_App = this;

        // init GLFW
        glfwInit();

        m_Window = std::make_shared<Window>(params.WindowParams);
        m_Window->Create();

        m_EventDispatcher = std::make_shared<EventDispatcher>();
    }

    Application::~Application() {
        m_Window->Destroy();

        // terminate GLFW
        glfwTerminate();
    }

    void Application::Run() {
        m_Running = true;

        float lastFrameTIme = GetTime();

        // register GLFW event callbacks
        glfwSetWindowUserPointer(m_Window->GetHandle(), s_App);

        glfwSetKeyCallback(m_Window->GetHandle(), OnSetKeyCallback);
        glfwSetMouseButtonCallback(m_Window->GetHandle(), OnSetMouseButtonCallback);

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

            // dispatch events to layers
            m_EventDispatcher->Dispatch();

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

    std::shared_ptr<Window> Application::GetWindow() {
        return m_Window;
    }

    std::shared_ptr<EventDispatcher> Application::GetEventDispatcher() {
        return m_EventDispatcher;
    }

    glm::vec2 Application::GetCursorPos() const {
        return m_Window->GetCursorPos();
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

    void Application::OnSetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if(action == GLFW_PRESS) {
            application->GetEventDispatcher()->PushEvent({ EventType::KeyPressed, key, mods });
        } else if(action == GLFW_RELEASE) {
            application->GetEventDispatcher()->PushEvent({ EventType::KeyReleased, key, mods });
        }
    }

    void Application::OnSetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if(action == GLFW_PRESS) {
            application->GetEventDispatcher()->PushEvent({ EventType::MouseButtonPressed, button, mods });
        } else if(action == GLFW_RELEASE) {
            application->GetEventDispatcher()->PushEvent({ EventType::MouseButtonReleased, button, mods });
        }
    }

}