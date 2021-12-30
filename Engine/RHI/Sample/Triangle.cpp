//
// Created by johnk on 29/12/2021.
//

#include <vector>

#include <GLFW/glfw3.h>
#include <RHI/Instance.h>
#include <RHI/PhysicalDevice.h>
#include <RHI/LogicalDevice.h>

using namespace RHI;

GLFWwindow* window;
Instance* instance;
std::vector<PhysicalDevice*> physicalDevices;
LogicalDevice* logicalDevice;

void Init()
{
    InstanceCreateInfo createInfo {};
    createInfo.debugMode = false;
    instance = RHI::Instance::CreateByPlatform(createInfo);
    physicalDevices.resize(instance->CountPhysicalDevices());

    for (uint32_t i = 0; i < physicalDevices.size(); i++) {
        physicalDevices[i] = instance->GetPhysicalDevice(i);
        if (physicalDevices[i]->GetProperty().isSoftware) {
            continue;
        }
        logicalDevice = instance->CreateLogicalDevice(physicalDevices[i]);
        break;
    }
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
