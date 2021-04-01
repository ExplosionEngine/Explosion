//
// Created by John Kindem on 2021/3/30.
//

#include <Application/Application.h>

#include <utility>

namespace Explosion {
    Application::Application(std::string name, uint32_t width, uint32_t height)
        : name(std::move(name)), width(width), height(height), window(nullptr) {}

    Application::~Application() = default;

    void Application::Run()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        OnStart();

        while (!glfwWindowShouldClose(window)) {
            OnDrawFrame();
            glfwPollEvents();
        }

        OnStop();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWwindow* Application::GetWindow() const
    {
        return window;
    }

    uint32_t Application::GetWidth() const
    {
        return width;
    }

    uint32_t Application::GetHeight() const
    {
        return height;
    }
}
