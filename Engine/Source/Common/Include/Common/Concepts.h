//
// Created by johnk on 2024/8/16.
//

#pragma once

#include <type_traits>

namespace Common {
    template <typename T> concept CppIntegral = std::is_integral_v<T>;
    template <typename T> concept CppFloatingPoint = std::is_floating_point_v<T>;
    template <typename T> concept CppSigned = std::is_signed_v<T>;
    template <typename T> concept CppUnsigned = std::is_unsigned_v<T>;
    template <typename T> concept CppArithmetic = std::is_arithmetic_v<T>;
    template <typename T> concept CppClass = std::is_class_v<T>;
    template <typename T> concept CppVoid = std::is_void_v<T>;
    template <typename T> concept CppUnion = std::is_union_v<T>;
    template <typename T> concept CppArray = std::is_array_v<T>;
    template <typename T> concept CppEnum = std::is_enum_v<T>;
    template <typename T> concept CppFunction = std::is_function_v<T>;
    template <typename T> concept CppInvocable = std::is_invocable_v<T>;
    template <typename T> concept CppPointer = std::is_pointer_v<T>;
    template <typename T> concept CppRef = std::is_reference_v<T>;
    template <typename T> concept CppLValueRef = std::is_lvalue_reference_v<T>;
    template <typename T> concept CppRValueRef = std::is_rvalue_reference_v<T>;
    template <typename T> concept CppConst = std::is_const_v<T>;
    template <typename T> concept CppVolatile = std::is_volatile_v<T>;
    template <typename T> concept CppCopyConstructible = std::is_copy_constructible_v<T>;
    template <typename T> concept CppMoveConstructible = std::is_move_constructible_v<T>;
    template <typename T> concept CppCopyAssignable = std::is_copy_assignable_v<T>;
    template <typename T> concept CppMoveAssignable = std::is_move_assignable_v<T>;
    template <uint8_t N, typename... T> concept ArgsNumEqual = sizeof...(T) == N;
    template <uint8_t N, typename... T> concept ArgsNumLess = sizeof...(T) < N;
    template <uint8_t N, typename... T> concept ArgsNumGreater = sizeof...(T) > N;
    template <uint8_t N, typename... T> concept ArgsNumLessEqual = sizeof...(T) <= N;
    template <uint8_t N, typename... T> concept ArgsNumGreaterEqual = sizeof...(T) >= N;
    template <typename C, typename B> concept DerivedFrom = std::is_base_of_v<B, C>;
}
