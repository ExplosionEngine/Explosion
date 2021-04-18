//
// Created by John Kindem on 2021/4/8.
//

#include <stdexcept>

#include <Explosion/Driver/Platform.h>

#ifdef WIN32
#include <windows.h>
#undef min
#undef max
#include <vulkan/vulkan_win32.h>

const char* INSTANCE_EXTENSIONS[] = {
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
};

uint32_t GetPlatformInstanceExtensionNum()
{
    return sizeof(INSTANCE_EXTENSIONS) / sizeof(const char*);
}

const char** GetPlatformInstanceExtensions()
{
    return INSTANCE_EXTENSIONS;
}

bool CreatePlatformSurface(const VkInstance& vkInstance, void* surface, VkSurfaceKHR& vkSurface)
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {};
    surfaceCreateInfo.sType= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = static_cast<HWND>(surface);
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &vkSurface) != VK_SUCCESS) {
        return false;
    }
    return true;
}
#endif
