//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_COMMON_H
#define EXPLOSION_COMMON_H

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.h>

namespace Explosion {
    std::optional<uint32_t> FindMemoryType(VkPhysicalDeviceMemoryProperties physicalDeviceProps, uint32_t typeFilter, VkMemoryPropertyFlags memProps);
}

#endif //EXPLOSION_COMMON_H
