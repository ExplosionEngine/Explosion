//
// Created by Administrator on 2021/4/23 0023.
//

#include <Explosion/Driver/Platform.h>

#ifdef __linux__
const char* INSTANCE_EXTENSIONS[] = {
    // TODO
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
    // TODO
    return false;
}
#endif
