#include <Render/Window.h>
using namespace Explosion;

Window* Window::Create()
{
    return new Window();
}

Window::~Window()
{
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
}

bool Window::Init(int width, int height, const char *name)
{
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    if (!glfwInit()) {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (window == nullptr) {
        return false;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return false;
    }
    glViewport(0, 0, width, height);

    this->width = width;
    this->height = height;
    this->name = name;
    return true;
}

bool Window::IsValid()
{
    return window != nullptr && !glfwWindowShouldClose(window);
}

void Window::Flush()
{
    if (window != nullptr) {
        glfwSwapBuffers(window);
    }
    glfwPollEvents();
}