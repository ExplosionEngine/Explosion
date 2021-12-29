//
// Created by johnk on 29/12/2021.
//

#include <GLFW/glfw3.h>
#include <RHI/Instance.h>

GLFWwindow* window;
RHI::Instance* instance;

void Init()
{
    // TODO
    instance = RHI::Instance::CreateByPlatform();
}

void DrawFrame()
{
    // TODO
}

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, "RHI-Triangle", nullptr, nullptr);

    Init();
    while (!glfwWindowShouldClose(window))
    {
        DrawFrame();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
