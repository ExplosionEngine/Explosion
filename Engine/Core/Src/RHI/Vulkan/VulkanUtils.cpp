//
// Created by John Kindem on 2021/4/29.
//

#include <Engine/RHI/Vulkan/VulkanUtils.h>

namespace Explosion::RHI {
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
