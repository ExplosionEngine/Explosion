//
// Created by John Kindem on 2021/3/30.
//

#include <Application/Application.h>

#include <utility>

namespace Explosion {
    Application::Application(void* window, std::string name, uint32_t width, uint32_t height)
        : window(window), name(std::move(name)), width(width), height(height), glfwWindow(nullptr) {}

    Application::~Application() = default;

    void Application::Run()
    {
        // TODO
    }

    void* Application::GetWindow() const
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
