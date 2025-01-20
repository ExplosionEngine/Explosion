//
// Created by johnk on 2023/5/10.
//

#pragma once

#include <cmath>
#include <numbers>

#include <Common/Concepts.h>

namespace Common {
    static constexpr float epsilon = 0.000001f;
    static constexpr float halfEpsilon = 0.001f;
    static constexpr float pi = std::numbers::pi_v<float>;
}

namespace Common {
    template <typename T> bool CompareNumber(T lhs, T rhs);
    template <CppIntegral T> T DivideAndRoundUp(T lhs, T rhs);
}

namespace Common {
    template <typename T>
    bool CompareNumber(T lhs, T rhs)
    {
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(lhs - rhs) < epsilon;
        } else {
            return lhs == rhs;
        }
    }

    template <CppIntegral T>
    T DivideAndRoundUp(T lhs, T rhs)
    {
        return (lhs + rhs - 1) / rhs;
    }
}
