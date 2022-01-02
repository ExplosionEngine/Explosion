//
// Created by johnk on 29/12/2021.
//

#include <vector>

#include <GLFW/glfw3.h>

#include <RHI/Enum.h>
#include <RHI/Instance.h>
#include <RHI/PhysicalDevice.h>
#include <RHI/LogicalDevice.h>
#include <RHI/Queue.h>

using namespace RHI;

GLFWwindow* window;
Instance* instance;
std::vector<PhysicalDevice*> physicalDevices;
LogicalDevice* logicalDevice;
Queue* graphicsQueue;

void Init()
{
    {
        std::vector<const char*> extensions = {
            RHI_INSTANCE_EXT_NAME_SURFACE.c_str(),
            RHI_INSTANCE_EXT_NAME_WINDOWS_SURFACE.c_str()
        };

        InstanceCreateInfo createInfo {};
        createInfo.debugMode = false;
        createInfo.extensionNum = extensions.size();
        createInfo.extensions = extensions.data();
        instance = Instance::CreateByPlatform(&createInfo);
    }

    {
        physicalDevices.resize(instance->CountPhysicalDevices());
        for (uint32_t i = 0; i < physicalDevices.size(); i++) {
            physicalDevices[i] = instance->GetPhysicalDevice(i);
            if (physicalDevices[i]->GetProperty().isSoftware) {
                continue;
            }

            QueueFamilyCreateInfo queueFamilyCreateInfo {};
            queueFamilyCreateInfo.type = QueueFamilyType::GRAPHICS;
            queueFamilyCreateInfo.queueNum = 1;

            LogicalDeviceCreateInfo logicalDeviceCreateInfo {};
            logicalDeviceCreateInfo.queueFamilyNum = 1;
            logicalDeviceCreateInfo.queueFamilyCreateInfos = &queueFamilyCreateInfo;
            logicalDevice = instance->CreateLogicalDevice(physicalDevices[i], &logicalDeviceCreateInfo);
            break;
        }
        graphicsQueue = logicalDevice->GetCommandQueue(QueueFamilyType::GRAPHICS, 0);
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
