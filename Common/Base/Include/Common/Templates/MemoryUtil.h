//
// Created by Zach Lee on 2021/8/23.
//

#ifndef EXPLOSION_MEMORY_UTIL_H
#define EXPLOSION_MEMORY_UTIL_H

#include <cstdint>
#include <type_traits>

namespace Explosion {

    template <typename T>
    inline constexpr T Alignment(T v, int64_t align)
    {
        static_assert(std::is_integral_v<T> || std::is_pointer_v<T>, "T shout be integer or pointer");
        return static_cast<T>(static_cast<int64_t>(v) + align - 1) & (~(align - 1));
    }

    inline constexpr bool IsPowerOfTwo(uint64_t x) {
        return x > 0 && !(x & (x - 1));
    }

}

#endif

