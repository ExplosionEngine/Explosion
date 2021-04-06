//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/SwapChain.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_MAC
#include <vulkan/vulkan_macos.h>
#endif

namespace Explosion {
    SwapChain::SwapChain(Device& device, void* surface, uint32_t width, uint32_t height)
        : device(device), surface(surface), width(width), height(height)
    {
        CreateSurface();
    }

    SwapChain::~SwapChain()
    {
        DestroySurface();
    }

    void SwapChain::CreateSurface()
    {
#ifdef TARGET_OS_MAC
        VkMacOSSurfaceCreateInfoMVK createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pView = surface;
        createInfo.pNext = nullptr;

        if (vkCreateMacOSSurfaceMVK(device.GetVkInstance(), &createInfo, nullptr, &vkSurface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create MacOS surface");
        }
#endif

        // TODO Windows Surface
    }

    void SwapChain::DestroySurface()
    {
        vkDestroySurfaceKHR(device.GetVkInstance(), vkSurface, nullptr);
    }
}
