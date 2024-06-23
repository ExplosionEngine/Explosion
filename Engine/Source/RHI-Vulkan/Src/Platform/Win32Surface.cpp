//
// Created by Zach Lee on 2022/4/4.
//

#if PLATFORM_WINDOWS
#include <windows.h>

#include <RHI/Surface.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <Common/Debug.h>

namespace RHI::Vulkan {
    VkSurfaceKHR CreateNativeSurface(const VkInstance& instance, const SurfaceCreateInfo& createInfo)
    {
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = static_cast<HWND>(createInfo.window);
        surfaceInfo.hinstance = GetModuleHandle(nullptr);
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        Assert(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface) == VK_SUCCESS);
        return surface;
    }
}
#endif