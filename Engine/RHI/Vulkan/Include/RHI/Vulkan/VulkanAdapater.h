//
// Created by John Kindem on 2021/4/23 0023.
//

#ifndef EXPLOSION_VULKANADAPATER_H
#define EXPLOSION_VULKANADAPATER_H

#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include <RHI/Enum.h>
#include <RHI/Vulkan/Api.h>

namespace Explosion::RHI {
    RHI_VULKAN_API VkBool32 VkBoolConvert(bool value);

    template <typename Type, typename VkType>
    RHI_VULKAN_API VkType VkConvert(const Type& type);

    template <typename VkType, typename Type>
    RHI_VULKAN_API Type GetEnumByVk(const VkType& vkType);

    template <typename VkType, typename Type>
    RHI_VULKAN_API VkFlags VkGetFlags(Flags flags);
}

#endif //EXPLOSION_VULKANADAPATER_H
