//
// Created by John Kindem on 2021/4/8.
//

#include <Explosion/Driver/Platform.h>

#ifdef WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>

const char* INSTANCE_EXTENSIONS[] = {
    VK_MVK_MACOS_SURFACE_EXTENSION_NAME
};

uint32_t GetPlatformInstanceExtensionNum()
{
    return sizeof(INSTANCE_EXTENSIONS) / sizeof(const char*);
}

const char** GetPlatformInstanceExtensions()
{
    return INSTANCE_EXTENSIONS;
}
#endif
