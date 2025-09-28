//
// Created by johnk on 9/4/2022.
//

#pragma once

#include <string>
#include <cstdint>

#define Assert(expression) Common::Debug::AssertImpl(expression, #expression, __FILE__, __LINE__)
#define AssertWithReason(expression, reason) Common::Debug::AssertImpl(expression, #expression, __FILE__, __LINE__, reason)
#define Unimplement() Assert(false)
#define QuickFail() Assert(false)
#define QuickFailWithReason(reason) AssertWithReason(false, reason)

#if COMPILER_MSVC
#define ENABLE_OPTIMIZATION __pragma(optimize("", on))
#define DISABLE_OPTIMIZATION __pragma(optimize("", off))
#elif COMPILER_APPLE_CLANG
#define ENABLE_OPTIMIZATION _Pragma("clang optimize on")
#define DISABLE_OPTIMIZATION _Pragma("clang optimize off")
#endif

#if !defined(__clang__)
    #define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL __pragma(optimize("",off))
    #define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL  __pragma(optimize("",on))
#elif defined(_MSC_VER)		// Clang only supports __pragma with -fms-extensions
    #define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL __pragma(clang optimize off)
    #define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL  __pragma(clang optimize on)
#endif

namespace Common {
    class Debug {
    public:
        static void AssertImpl(bool expression, const std::string& name, const std::string& file, uint32_t line, const std::string& reason = "");

        ~Debug();

    private:
        Debug();
    };

    // for compile-time check type from compile error
    // sample:
    //     TypeDiagnosis<decltype(x)> xType;
    template <typename T> class TypeDiagnosis;
}
