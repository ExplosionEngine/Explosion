//
// Created by LiZhen on 2021/8/22 0016.
//

#ifndef EXPLOSION_DEVICEINFO_H
#define EXPLOSION_DEVICEINFO_H

#include <cstdint>
#include <string_view>
#include <RHI/Enum.h>

namespace Explosion::RHI {

    struct DeviceFeature {
        bool tessellation;
        bool geometry;
    };

    struct DeviceLimit {
        uint32_t maxPushConstantSize;
        uint32_t maxColorAttachments;
        uint32_t minMemoryMapAlignment;
        uint32_t minTexelOffsetAlignment;
        uint32_t minUboOffsetAlignment;
        uint32_t minSboOffsetAlignment;
    };

    struct DeviceInfo {
        uint32_t vender;
        uint32_t device;
        DeviceType type;
        std::string_view deviceName;
        DeviceLimit limits;
        DeviceFeature features;
    };
}

#endif