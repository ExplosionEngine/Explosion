//
// Created by johnk on 13/3/2022.
//

#pragma once

#include <string>

#define ARCH_X86 0
#define ARCH_ARM 0

#if defined(__x86_64__) || defined(_M_X64)
#undef ARCH_X86
#define ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#undef ARCH_ARM
#define ARCH_ARM 1
#endif

namespace Common {
    class PlatformUtils {
    public:
        static void SetEnvVar(const std::string& inKey, const std::string& inValue);
    };
}
