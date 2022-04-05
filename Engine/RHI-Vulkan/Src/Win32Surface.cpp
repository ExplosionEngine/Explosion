//
// Created by Zach Lee on 2022/4/4.
//

#if defined(WIN32)
#include <RHI/SwapChain.h>
#include <RHI/Vulkan/Exception.h>
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {

    vk::SurfaceKHR CreateNativeSurface(vk::Instance& instance, const SwapChainCreateInfo* createInfo)
    {
        vk::Win32SurfaceCreateInfoKHR surfaceInfo {};
        surfaceInfo.setHwnd((HWND)createInfo->window)
            .setHinstance(GetModuleHandle(0));
        vk::SurfaceKHR surface = VK_NULL_HANDLE;
        if (instance.createWin32SurfaceKHR(&surfaceInfo, nullptr, &surface) != vk::Result::eSuccess) {
            throw VKException("failed to create win32 surface");
        }
        return surface;
    }
}

#endif