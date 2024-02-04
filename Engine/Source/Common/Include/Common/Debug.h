//
// Created by johnk on 9/4/2022.
//

#pragma once

#include <string>
#include <cstdint>
#include <cassert>
#include <iostream>

#if BUILD_CONFIG_DEBUG
#include <debugbreak.h>
#endif

#define Assert(expression) Common::Debug::AssertImpl(expression, #expression, __FILE__, __LINE__)
#define AssertWithReason(expression, reason) Common::Debug::AssertImpl(expression, #expression, __FILE__, __LINE__, reason)
#define Unimplement() Assert(false)
#define QuickFail() Assert(false)

namespace Common {
    class Debug {
    public:
        static void AssertImpl(bool expression, const std::string& name, const std::string& file, uint32_t line, const std::string& reason = "")
        {
            if (expression) {
                return;
            }
            std::cerr << "Assert failed: " << name << ", " << file << ", " << line << std::endl;
            std::cerr << "Reason: " << reason << std::endl;

#if BUILD_CONFIG_DEBUG
            debug_break();
#endif
        }

        ~Debug() = default;

    private:

        Debug() = default;
    };
}
