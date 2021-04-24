//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_ENUMADAPTER_H
#define EXPLOSION_ENUMADAPTER_H

#include <stdexcept>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
    template <typename Type, typename VkType>
    VkType VkConvert(const Type& type)
    {
        throw std::runtime_error("failed to find suitable template instance");
    }
}

#endif //EXPLOSION_ENUMADAPTER_H
