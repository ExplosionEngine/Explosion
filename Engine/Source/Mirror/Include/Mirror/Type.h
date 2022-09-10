//
// Created by johnk on 2022/9/4.
//

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <functional>

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
