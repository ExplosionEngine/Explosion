//
// Created by John Kindem on 2021/4/12.
//

#ifndef EXPLOSION_PLATFORM_H
#define EXPLOSION_PLATFORM_H

#include <vulkan/vulkan.h>

// TODO Win32

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_MAC
#include <vulkan/vulkan_macos.h>
#endif

uint32_t GetPlatformInstanceExtensionNum();
const char** GetPlatformInstanceExtensions();

#endif //EXPLOSION_PLATFORM_H
