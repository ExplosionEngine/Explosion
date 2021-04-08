//
// Created by John Kindem on 2021/4/8.
//

#ifndef EXPLOSION_PLATFORM_H
#define EXPLOSION_PLATFORM_H

#include <vulkan/vulkan.h>

uint32_t GetPlatformInstanceExtensionNum();
const char** GetPlatformInstanceExtensions();

bool CreatePlatformSurface(const VkInstance& vkInstance, void* surface, VkSurfaceKHR& vkSurface);

#endif //EXPLOSION_PLATFORM_H
