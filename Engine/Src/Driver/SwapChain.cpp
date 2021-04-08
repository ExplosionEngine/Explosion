//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/SwapChain.h>
#include <Explosion/Driver/Platform.h>

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
        if (!CreatePlatformSurface(device.GetVkInstance(), surface, vkSurface)) {
            throw std::runtime_error("failed to create vulkan surface");
        }
    }

    void SwapChain::DestroySurface()
    {
        vkDestroySurfaceKHR(device.GetVkInstance(), vkSurface, nullptr);
    }
}
