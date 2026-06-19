//
// Created by johnk on 9/4/2022.
//

#pragma once

#include <string>
#include <string_view>
#include <cstdint>

#define Assert(expression) Common::Debug::AssertImpl((expression), #expression, __FILE__, __LINE__)
#define AssertWithReason(expression, reason) Common::Debug::AssertImpl((expression), #expression, __FILE__, __LINE__, (reason))
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
        // The passing case lives here and is inlined, so the optimizer can see through it: a compile-time-true
        // condition folds away entirely, and a runtime one collapses to a single predicted-not-taken branch with no
        // std::string construction and no heap allocation. Only an actual failure reaches the out-of-line cold path.
        static void AssertImpl(bool expression, const char* name, const char* file, uint32_t line,
            std::string_view reason = {})
        {
            if (expression) {
                return;
            }
            AssertFailed(name, file, line, reason);
        }

        ~Debug();

    private:
        static void AssertFailed(const char* name, const char* file, uint32_t line, std::string_view reason);

        Debug();
    };

    // for compile-time check type from compile error
    // sample:
    //     TypeDiagnosis<decltype(x)> xType;
    template <typename T> class TypeDiagnosis;
}
