#include "Window.h"

#include <glad/gl.h>

#include <assert.h>
#include <iostream>

namespace Core {

    Window::Window(const WindowParams& params) 
        : m_Params(params) {
    }

    Window::~Window() {
        Destroy();
    }

    void Window::Create() {
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        // create GLFW window
        m_Handle = glfwCreateWindow(m_Params.Width, m_Params.Height, m_Params.Title.c_str(), nullptr, nullptr);

        if(!m_Handle) {
            std::cerr << "Failed to create GLFW window\n";
            assert(false);
        }

        // init GLAD
        glfwMakeContextCurrent(m_Handle);
        gladLoadGL(glfwGetProcAddress);

        glfwSwapInterval(m_Params.VSync ? 1 : 0);

        glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    
    void Window::Destroy() {
        if(m_Handle) {
            glfwDestroyWindow(m_Handle);
        }

        m_Handle = nullptr;
    }

    void Window::Update() {
        glfwSwapBuffers(m_Handle);

        // lock mouse position (TODO: add a boolean parameter)
        // I think it is the wrong way of doing it...
        glfwSetCursorPos(m_Handle, m_Params.Width / 2, m_Params.Height / 2);
    }

    GLFWwindow* Window::GetHandle() {
        return m_Handle;
    }

    glm::vec2 Window::GetCursorPos() {
        double xPosition, yPosition;
        glfwGetCursorPos(m_Handle, &xPosition, &yPosition);
        return { (float)xPosition, (float)yPosition }; 
    }

    glm::vec2 Window::GetFrameBufferSize() {
        int width, height;
        glfwGetFramebufferSize(m_Handle, &width, &height);
        return { width, height };
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Handle) != 0;
    }

}