//
// Created by johnk on 13/3/2022.
//

#pragma once

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
        switch,
        max
    };

    enum class CpuArch {
        x86,
        x64,
        max
    };
}
