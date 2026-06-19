//
// Created by johnk on 2024/4/14.
//

#if BUILD_CONFIG_DEBUG
#include <debugbreak.h>
#endif

#include <Common/Debug.h>
#include <Common/IO.h>

namespace Common {
    void Debug::AssertFailed(const char* name, const char* file, const uint32_t line, const std::string_view reason)
    {
        AutoCerrFlush;

        std::cerr << "Assert failed: " << name << ", " << file << ", " << line << newline;
        std::cerr << "Reason: " << reason << newline;

#if BUILD_CONFIG_DEBUG
        debug_break();
#endif
    }

    Debug::Debug() = default;

    Debug::~Debug() = default;
}
