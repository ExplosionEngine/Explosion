//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_VKADAPATER_H
#define EXPLOSION_VKADAPATER_H

#include <stdexcept>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
    VkBool32 VkBoolConvert(bool value);

    template <typename Type, typename VkType>
    VkType VkConvert(const Type& type);

    template <typename VkType, typename Type>
    Type GetEnumByVk(const VkType& vkType);
}

#endif //EXPLOSION_VKADAPATER_H
