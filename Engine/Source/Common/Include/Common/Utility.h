//
// Created by johnk on 2021/12/19.
//

#pragma once

#include <functional>
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <string>

#include <Common/Concepts.h>
#include <Common/Debug.h>

#define DefaultCopyCtor(clz) \
    clz(clz&) = default; \
    clz(const clz&) = default; \

#define DefaultCopyAssignOp(clz) \
    clz& operator=(clz&) = default; \
    clz& operator=(const clz&) = default; \

#define NonCopyCtor(clz) \
    clz(clz&) = delete; \
    clz(const clz&) = delete; \

#define NonCopyAssignOp(clz) \
    clz& operator=(clz&) = delete; \
    clz& operator=(const clz&) = delete; \

#define DefaultMovable(clz) \
    clz(clz&&) noexcept = default; \
    clz& operator=(clz&&) noexcept = default; \

#define NonMovable(clz) \
    clz(clz&&) = delete; \
    clz& operator=(clz&&) = delete; \

#define DefaultCopyable(clz) \
    DefaultCopyCtor(clz) \
    DefaultCopyAssignOp(clz) \

#define NonCopyable(clz) \
    NonCopyCtor(clz) \
    NonCopyAssignOp(clz) \

namespace Common {
    template <uint32_t A, CppIntegral T>
    T AlignUp(T value);

    template <typename LHS, typename... RHS>
    struct IsAllSame {};

    template <CppArithmetic T>
    T ToArithmetic(const std::string& inStr);

    template <typename T>
    struct Wrap {
        using RawType = T;
    };

    template <typename T>
    struct TupleTypeTraverser {};
}

// ------------------------ FlagAndBits Begin ------------------------
#define DECLARE_FLAG_BITS_OP(FlagsType, BitsType) \
    inline FlagsType operator&(BitsType a, BitsType b) { return FlagsType(static_cast<FlagsType::UnderlyingType>(a) & static_cast<FlagsType::UnderlyingType>(b)); } \
    inline FlagsType operator|(BitsType a, BitsType b) { return FlagsType(static_cast<FlagsType::UnderlyingType>(a) | static_cast<FlagsType::UnderlyingType>(b)); } \

namespace Common {
    template <typename E>
    using BitsForEachFunc = std::function<void(E e)>;

    template <typename E>
    void ForEachBits(BitsForEachFunc<E>&& func);

    template <typename E>
    class Flags {
    public:
        static Flags null;

        using UnderlyingType = std::underlying_type_t<E>;

        Flags();
        ~Flags();
        Flags(UnderlyingType inValue); // NOLINT
        Flags(E e); // NOLINT

        UnderlyingType Value() const;
        explicit operator bool();
        bool operator==(Flags other) const;
        bool operator!=(Flags other) const;
        bool operator==(UnderlyingType inValue) const;
        bool operator!=(UnderlyingType inValue) const;
        bool operator==(E e) const;
        bool operator!=(E e) const;
        Flags operator&(Flags other) const;
        Flags operator|(Flags other) const;
        Flags operator&(UnderlyingType inValue) const;
        Flags operator|(UnderlyingType inValue) const;
        Flags operator&(E e) const;
        Flags operator|(E e) const;

    private:
        UnderlyingType value;
    };
}
// ------------------------ FlagAndBits End --------------------------

namespace Common {
    template <uint32_t A, CppIntegral T>
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

    template <CppArithmetic T>
    T ToArithmetic(const std::string& inStr)
    {
        if constexpr (CppIntegral<T>) {
            return std::stoi(inStr);
        } else if constexpr (CppFloatingPoint<T>) {
            return std::stof(inStr);
        } else if constexpr (CppBool<T>) {
            if (inStr == "true") {
                return true;
            } else if (inStr == "false") {
                return false;
            } else {
                return QuickFail(), false;
            }
        } else {
            return QuickFail(), T();
        }
    }

    template <typename... T>
    struct TupleTypeTraverser<std::tuple<T...>> {
        template <typename F>
        static void Each(F&& inFunc)
        {
            (void) std::initializer_list<int> { ([&]() -> void {
                inFunc(T());
            }(), 0)... };
        }
    };

    template <typename E>
    void ForEachBits(BitsForEachFunc<E>&& func)
    {
        using UBitsType = std::underlying_type_t<E>;
        for (UBitsType i = 0x1; i < static_cast<UBitsType>(E::max); i = i << 1) {
            func(static_cast<E>(i));
        }
    }

    template <typename E>
    Flags<E>::Flags() = default;

    template <typename E>
    Flags<E>::~Flags() = default;

    template <typename E>
    Flags<E>::Flags(UnderlyingType inValue)
        : value(inValue)
    {
    }

    template <typename E>
    Flags<E>::Flags(E e)
        : value(static_cast<UnderlyingType>(e))
    {
    }

    template <typename E>
    typename Flags<E>::UnderlyingType Flags<E>::Value() const
    {
        return value;
    }

    template <typename E>
    Flags<E>::operator bool()
    {
        return value;
    }

    template <typename E>
    bool Flags<E>::operator==(Flags other) const
    {
        return value == other.value;
    }

    template <typename E>
    bool Flags<E>::operator!=(Flags other) const
    {
        return value != other.value;
    }

    template <typename E>
    bool Flags<E>::operator==(UnderlyingType inValue) const
    {
        return value == inValue;
    }

    template <typename E>
    bool Flags<E>::operator!=(UnderlyingType inValue) const
    {
        return value != inValue;
    }

    template <typename E>
    bool Flags<E>::operator==(E e) const
    {
        return value == static_cast<UnderlyingType>(e);
    }

    template <typename E>
    bool Flags<E>::operator!=(E e) const
    {
        return value != static_cast<UnderlyingType>(e);
    }

    template <typename E>
    Flags<E> Flags<E>::null = Flags<E>(0);

    template <typename E>
    Flags<E> Flags<E>::operator&(Flags other) const
    {
        return Flags<E>(Value() & other.Value());
    }

    template <typename E>
    Flags<E> Flags<E>::operator|(Flags other) const
    {
        return Flags<E>(Value() | other.Value());
    }

    template <typename E>
    Flags<E> Flags<E>::operator&(UnderlyingType inValue) const
    {
        return Flags<E>(Value() & inValue);
    }

    template <typename E>
    Flags<E> Flags<E>::operator|(UnderlyingType inValue) const
    {
        return Flags<E>(Value() | inValue);
    }

    template <typename E>
    Flags<E> Flags<E>::operator&(E e) const
    {
        return Flags<E>(Value() & static_cast<UnderlyingType>(e));
    }

    template <typename E>
    Flags<E> Flags<E>::operator|(E e) const
    {
        return Flags<E>(Value() | static_cast<UnderlyingType>(e));
    }
}

namespace std { // NOLINT
    template <typename E>
    struct hash<Common::Flags<E>>
    {
        size_t operator()(Common::Flags<E> flags) const
        {
            return hash<typename Common::Flags<E>::UnderlyingType>()(flags.Value());
        }
    };
}
