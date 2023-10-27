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
#include <Common/Hash.h>
#include <Common/Debug.h>

#if COMPILER_MSVC
#define functionSignature __FUNCSIG__
#else
#define functionSignature __PRETTY_FUNCTION__
#endif

namespace Mirror {
    using TypeId = size_t;

    static constexpr size_t typeIdNull = 0u;

    MIRROR_API inline TypeId ComputeTypeId(std::string_view sigName)
    {
        return Common::HashUtils::CityHash(sigName.data(), sigName.size());
    }

    struct TypeInfo {
#if BUILD_CONFIG_DEBUG
        // NOTICE: this name is platform relative, so do not use it unless for debug
        std::string debugName;
#endif
        TypeId id;
        const bool isConst;
        const bool isLValueReference;
        const bool isRValueReference;
        const bool isPointer;
        const bool isClass;
        std::function<TypeInfo*()> addConst;
        std::function<TypeInfo*()> removeConst;
        std::function<TypeInfo*()> removeRef;
        std::function<TypeInfo*()> removeCVRef;
        std::function<TypeInfo*()> removePointer;
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
            std::is_lvalue_reference_v<T>,
            std::is_rvalue_reference_v<T>,
            std::is_pointer_v<T>,
            std::is_class_v<T>,
            []() -> TypeInfo* { return GetTypeInfo<std::add_const_t<T>>(); },
            []() -> TypeInfo* { return GetTypeInfo<std::remove_const_t<T>>(); },
            []() -> TypeInfo* { return GetTypeInfo<std::remove_reference_t<T>>(); },
            []() -> TypeInfo* { return GetTypeInfo<std::remove_cvref_t<T>>(); },
            []() -> TypeInfo* { return GetTypeInfo<std::remove_pointer_t<T>>(); }
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
