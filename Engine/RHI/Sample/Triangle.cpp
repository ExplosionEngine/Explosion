//
// Created by johnk on 29/12/2021.
//

#include <vector>

#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif
#include <GLFW/glfw3native.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <RHI/Enum.h>
#include <RHI/Instance.h>
#include <RHI/PhysicalDevice.h>
#include <RHI/LogicalDevice.h>
#include <RHI/Surface.h>
#include <RHI/SwapChain.h>
#include <RHI/Queue.h>

using namespace RHI;

size_t windowWidth = 1024;
size_t windowHeight = 768;

GLFWwindow* window;
Instance* instance;
std::vector<PhysicalDevice*> physicalDevices;
LogicalDevice* logicalDevice;
Queue* graphicsQueue;
Surface* surface;
SwapChain* swapChain;

void Init()
{
    {
        std::vector<const char*> extensions = {
            RHI_INSTANCE_EXT_NAME_SURFACE.c_str(),
#ifdef _WIN32
            RHI_INSTANCE_EXT_NAME_WINDOWS_SURFACE.c_str(),
#endif
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

            std::vector<const char*> extensions = {
                RHI_DEVICE_EXT_NAME_SWAP_CHAIN.c_str()
            };

            LogicalDeviceCreateInfo logicalDeviceCreateInfo {};
            logicalDeviceCreateInfo.queueFamilyNum = 1;
            logicalDeviceCreateInfo.queueFamilyCreateInfos = &queueFamilyCreateInfo;
            logicalDeviceCreateInfo.extensionNum = extensions.size();
            logicalDeviceCreateInfo.extensions = extensions.data();
            logicalDevice = instance->CreateLogicalDevice(physicalDevices[i], &logicalDeviceCreateInfo);
            break;
        }
        graphicsQueue = logicalDevice->GetCommandQueue(QueueFamilyType::GRAPHICS, 0);
    }

    {
#ifdef _WIN32
        WindowsSurfaceCreateInfo wCreateInfo {};
        wCreateInfo.hWnd = glfwGetWin32Window(window);
        wCreateInfo.hInstance = GetModuleHandle(nullptr);
#endif

        SurfaceCreateInfo createInfo {};
#ifdef _WIN32
        createInfo.windows = &wCreateInfo;
#endif

        surface = instance->CreateSurface(&createInfo);
    }

    {
        SwapChainCreateInfo createInfo {};
        createInfo.surface = surface;
        createInfo.queue = graphicsQueue;
        createInfo.imageNum = 2;
        createInfo.extent = { windowWidth, windowHeight };
        createInfo.format = PixelFormat::R8G8B8A8_UNORM;

        swapChain = logicalDevice->CreateSwapChain(&createInfo);
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
    window = glfwCreateWindow(windowWidth, windowHeight, "RHI-Triangle", nullptr, nullptr);

    Init();
    while (!glfwWindowShouldClose(window))
    {
        DrawFrame();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
