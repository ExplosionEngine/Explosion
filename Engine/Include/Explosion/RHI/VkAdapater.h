//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_VKADAPATER_H
#define EXPLOSION_VKADAPATER_H

#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>

namespace Explosion {
    VkBool32 VkBoolConvert(bool value);

    template <typename Type, typename VkType>
    VkType VkConvert(const Type& type);

    template <typename VkType, typename Type>
    Type GetEnumByVk(const VkType& vkType);

    template<typename Type, typename VkType>
    VkFlags VkGetFlags(const std::vector<Type>& flagBits)
    {
        VkFlags flags = 0;
        for (auto& flagBit : flagBits) {
            flags |= VkConvert<Type, VkType>(flagBit);
        }
        return flags;
    }
}

#endif //EXPLOSION_VKADAPATER_H
