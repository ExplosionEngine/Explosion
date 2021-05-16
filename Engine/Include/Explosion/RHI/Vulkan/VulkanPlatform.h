//
// Created by John Kindem on 2021/4/8.
//

#ifndef EXPLOSION_VULKANPLATFORM_H
#define EXPLOSION_VULKANPLATFORM_H

#include <vulkan/vulkan.h>

namespace Explosion::RHI {
    uint32_t GetPlatformInstanceExtensionNum();
    const char** GetPlatformInstanceExtensions();

    bool CreatePlatformSurface(const VkInstance& vkInstance, void* surface, VkSurfaceKHR& vkSurface);
}

#endif //EXPLOSION_VULKANPLATFORM_H
