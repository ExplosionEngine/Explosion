//
// Created by johnk on 13/3/2022.
//

#pragma once

#include <string>

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
