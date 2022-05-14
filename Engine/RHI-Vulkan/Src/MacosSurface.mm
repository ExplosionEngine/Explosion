//
// Created by Zach Lee on 2022/4/4.
//

#if defined(__APPLE__)
#include <RHI/SwapChain.h>
#include <Cocoa/Cocoa.h>
#include <Common/Debug.h>

#define VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {

    vk::SurfaceKHR CreateNativeSurface(vk::Instance& instance, const SwapChainCreateInfo* createInfo)
    {
        auto view = static_cast<NSView*>(createInfo->window);

        vk::MacOSSurfaceCreateInfoMVK surfaceInfo {};
        surfaceInfo.setPView(view);
        vk::SurfaceKHR surface = VK_NULL_HANDLE;
        Assert(instance.createMacOSSurfaceMVK(&surfaceInfo, nullptr, &surface) == vk::Result::eSuccess);
        return surface;
    }
}

#endif