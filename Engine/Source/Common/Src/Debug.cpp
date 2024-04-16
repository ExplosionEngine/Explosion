//
// Created by johnk on 2024/4/14.
//

#include <Common/Debug.h>

namespace Common {
    void Debug::AssertImpl(bool expression, const std::string& name, const std::string& file, uint32_t line, const std::string& reason)
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

    Debug::Debug() = default;

    Debug::~Debug() = default;
}
