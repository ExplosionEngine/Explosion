//
// Created by John Kindem on 2021/4/8.
//

#ifndef EXPLOSION_VULKANPLATFORM_H
#define EXPLOSION_VULKANPLATFORM_H

#include <vulkan/vulkan.h>

#include <RHI/Vulkan/Api.h>

namespace Explosion::RHI {
    uint32_t RHI_VULKAN_API GetPlatformInstanceExtensionNum();
    const char** RHI_VULKAN_API GetPlatformInstanceExtensions();

    bool RHI_VULKAN_API CreatePlatformSurface(const VkInstance& vkInstance, void* surface, VkSurfaceKHR& vkSurface);
}

#endif //EXPLOSION_VULKANPLATFORM_H
