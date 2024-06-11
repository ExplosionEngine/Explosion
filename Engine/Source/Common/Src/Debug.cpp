//
// Created by johnk on 2024/4/14.
//

#if BUILD_CONFIG_DEBUG
#include <debugbreak.h>
#endif

#include <Common/Debug.h>
#include <Common/IO.h>

namespace Common {
    void Debug::AssertImpl(const bool expression, const std::string& name, const std::string& file, const uint32_t line, const std::string& reason)
    {
        AutoCerrFlush;

        if (expression) {
            return;
        }
        std::cerr << "Assert failed: " << name << ", " << file << ", " << line << newline;
        std::cerr << "Reason: " << reason << newline;

#if BUILD_CONFIG_DEBUG
        debug_break();
#endif
    }

    Debug::Debug() = default;

    Debug::~Debug() = default;
}
