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
    VkBool32 RHI_VULKAN_API VkBoolConvert(bool value);

    template <typename Type, typename VkType>
    VkType RHI_VULKAN_API VkConvert(const Type& type);

    template <typename VkType, typename Type>
    Type RHI_VULKAN_API GetEnumByVk(const VkType& vkType);

    template <typename VkType, typename Type>
    VkFlags RHI_VULKAN_API VkGetFlags(Flags flags);
}

#endif //EXPLOSION_VULKANADAPATER_H
