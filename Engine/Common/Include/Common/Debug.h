//
// Created by johnk on 9/4/2022.
//

#pragma once

#include <string>
#include <cstdint>
#if BUILD_CONFIG_DEBUG
#include <cassert>
#include <iostream>
#include <debugbreak.h>
#endif

#define Assert(expression) Common::Debug::AssertImpl(expression, #expression, __FILE__, __LINE__)

namespace Common {
    class Debug {
    public:
        static void AssertImpl(bool target, const std::string& name, const std::string& file, uint32_t line)
        {
#if BUILD_CONFIG_DEBUG
            if (target) {
                return;
            }
            std::cerr << "Assert failed: " << name << ", " << file << ", " << line << std::endl;
            debug_break();
#endif
        }

        ~Debug() = default;

    private:

        Debug() = default;
    };
}
