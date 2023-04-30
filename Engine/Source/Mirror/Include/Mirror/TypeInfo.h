//
// Created by johnk on 2022/9/4.
//

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <functional>
#include <tuple>
#include <array>

#include <Mirror/Api.h>

#if COMPILER_MSVC
#define functionSignature __FUNCSIG__
#else
#define functionSignature __PRETTY_FUNCTION__
#endif

namespace Mirror {
    using TypeId = size_t;

    MIRROR_API TypeId ComputeTypeId(std::string_view sigName);

    struct TypeInfo {
#if BUILD_CONFIG_DEBUG
        // NOTICE: this name is platform relative, so do not use it unless in for debug
        std::string debugName;
#endif
        TypeId id;
        const bool isConst;
        std::function<TypeInfo*()> removeConst;
    };

    template <typename T>
    TypeInfo* GetTypeInfo()
    {
        static TypeInfo typeInfo = {
#if BUILD_CONFIG_DEBUG
            functionSignature,
#endif
            ComputeTypeId(functionSignature),
            std::is_const_v<T>,
            []() -> TypeInfo* { return GetTypeInfo<std::remove_const_t<T>>(); }
        };
        return &typeInfo;
    }
}

namespace Mirror {
    template <typename T>
    struct VariableTraits {};

    template <typename T>
    struct VariableTraits<T*> {
        using ValueType = T;
    };

    template <typename T>
    struct FunctionTraits {};

    template <typename Ret, typename... Args>
    struct FunctionTraits<Ret(*)(Args...)> {
        using RetType = Ret;
        using ArgsTupleType = std::tuple<Args...>;
    };

    template <typename T>
    struct MemberVariableTraits {};

    template <typename Class, typename T>
    struct MemberVariableTraits<T Class::*> {
        using ClassType = Class;
        using ValueType = T;
    };

    template <typename Class, typename T>
    struct MemberVariableTraits<T Class::* const> {
        using ClassType = const Class;
        using ValueType = T;
    };

    template <typename T>
    struct MemberFunctionTraits {};

    template <typename Class, typename Ret, typename... Args>
    struct MemberFunctionTraits<Ret(Class::*)(Args...)> {
        using ClassType = Class;
        using RetType = Ret;
        using ArgsTupleType = std::tuple<Args...>;
    };

    template <typename Class, typename Ret, typename... Args>
    struct MemberFunctionTraits<Ret(Class::*)(Args...) const> {
        using ClassType = const Class;
        using RetType = Ret;
        using ArgsTupleType = std::tuple<Args...>;
    };
}

namespace Mirror {
    template <typename T>
    concept IsLValueRef = std::is_rvalue_reference_v<T>;

    template <typename T>
    concept IsRValueRef = std::is_rvalue_reference_v<T>;
}
