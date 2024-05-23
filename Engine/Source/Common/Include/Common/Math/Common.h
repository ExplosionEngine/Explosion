//
// Created by johnk on 2023/5/10.
//

#pragma once

#include <cmath>

namespace Common {
    static constexpr float epsilon = 0.000001f;
    static constexpr float halfEpsilon = 0.001f;
    static constexpr float pi = 3.1415926f;
}

namespace Common {
    template <typename T>
    T CompareNumber(T lhs, T rhs)
    {
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(lhs - rhs) < epsilon;
        } else {
            return lhs == rhs;
        }
    }
}
