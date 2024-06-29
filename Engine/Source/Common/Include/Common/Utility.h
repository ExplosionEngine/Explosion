//
// Created by johnk on 2021/12/19.
//

#pragma once

#include <cstdint>
#include <type_traits>

#define NonCopyable(clz) \
    clz(clz&) = delete; \
    clz& operator=(clz&) = delete; \
    clz(const clz&) = delete; \
    clz& operator=(const clz&) = delete; \

namespace Common {
    template <uint32_t A, typename T>
    requires std::is_integral_v<T>
    T AlignUp(T value);

    template <typename LHS, typename... RHS>
    struct IsAllSame {};
}

namespace Common {
    template <uint32_t A, typename T> requires std::is_integral_v<T>
    T AlignUp(T value)
    {
        return (value + (A - 1)) & ~(A - 1);
    }

    template <typename LHS, typename RHS0, typename... RHS>
    struct IsAllSame<LHS, RHS0, RHS...> {
        static constexpr bool value = std::is_same_v<std::remove_cvref_t<LHS>, std::remove_cvref_t<RHS0>> && IsAllSame<LHS, RHS...>::value;
    };

    template <typename LHS, typename RHS>
    struct IsAllSame<LHS, RHS> {
        static constexpr bool value = std::is_same_v<std::remove_cvref_t<LHS>, std::remove_cvref_t<RHS>>;
    };
}
