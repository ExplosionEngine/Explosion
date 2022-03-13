//
// Created by johnk on 13/3/2022.
//

#ifndef EXPLOSION_COMMON_PLATFORM_H
#define EXPLOSION_COMMON_PLATFORM_H

namespace Common {
    enum class DevelopmentPlatform {
        WINDOWS,
        MACOS,
        LINUX,
        MAX
    };

    enum class TargetPlatform {
        WINDOWS,
        MACOS,
        LINUX,
        ANDROID,
        IOS,
        XBOX,
        PLAY_STATION,
        SWITCH,
        MAX
    };

    enum class CpuArch {
        X86,
        X64,
        MAX
    };
}

#endif//EXPLOSION_COMMON_PLATFORM_H
