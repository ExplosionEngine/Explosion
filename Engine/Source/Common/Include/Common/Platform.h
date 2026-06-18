//
// Created by johnk on 13/3/2022.
//

#pragma once

#include <string>

// GCC and Clang in GNU mode predefine the bare macro `linux` as 1, which collides with the linux enumerators below.
// Drop it so the lowercase enumerator stays valid; the canonical `__linux__` is unaffected.
#ifdef linux
#undef linux
#endif

namespace Common {
    enum class DevelopmentPlatform {
        windows,
        macos,
        linux,
        max
    };

    enum class TargetPlatform {
        windows,
        macos,
        linux,
        android,
        ios,
        xbox,
        playStation,
        nintendoSwitch,
        max
    };

    enum class CpuArch {
        x86,
        x64,
        max
    };

    class PlatformUtils {
    public:
        static void SetEnvVar(const std::string& inKey, const std::string& inValue);
    };
}
