//
// Created by johnk on 13/3/2022.
//

#pragma once

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
