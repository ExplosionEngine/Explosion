//
// Created by johnk on 2022/9/4.
//

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace Mirror {
    using TypeHash = size_t;

    TypeHash CalculateTypeHash(const std::string& name);

    struct TypeInfo {
        bool isConst;
        bool isPointer;
        bool isLValueRef;
        bool isRValueRef;
    };

    template <typename T>
    TypeInfo* GetTypeInfo()
    {
        static TypeInfo typeInfo = {
            std::is_const_v<T>,
            std::is_pointer_v<T>,
            std::is_lvalue_reference_v<T>,
            std::is_rvalue_reference_v<T>
        };
        return &typeInfo;
    }
}
