//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_COMMON_H
#define EXPLOSION_COMMON_H

#include <cstdint>
#include <optional>

namespace Explosion {
    std::optional<uint32_t> FindMemoryType(VkPhysicalDeviceMemoryProperties physicalDeviceProps, uint32_t typeFilter, VkMemoryPropertyFlags memProps)
    {
        std::optional<uint32_t> result;
        for (uint32_t i = 0; i < physicalDeviceProps.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i))
                && (physicalDeviceProps.memoryTypes[i].propertyFlags & memProps) == memProps) {
                result = i;
                break;
            }
        }
        return result;
    }
}

#endif //EXPLOSION_COMMON_H
