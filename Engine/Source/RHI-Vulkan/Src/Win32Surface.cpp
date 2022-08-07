//
// Created by Zach Lee on 2022/4/4.
//

#if PLATFORM_WINDOWS
#include <RHI/SwapChain.h>
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <Common/Debug.h>

namespace RHI::Vulkan {

    vk::SurfaceKHR CreateNativeSurface(const vk::Instance& instance, const SwapChainCreateInfo* createInfo)
    {
        vk::Win32SurfaceCreateInfoKHR surfaceInfo {};
        surfaceInfo.setHwnd((HWND)createInfo->window)
            .setHinstance(GetModuleHandle(0));
        vk::SurfaceKHR surface = VK_NULL_HANDLE;
        Assert(instance.createWin32SurfaceKHR(&surfaceInfo, nullptr, &surface) == vk::Result::eSuccess);
        return surface;
    }
}
#endif