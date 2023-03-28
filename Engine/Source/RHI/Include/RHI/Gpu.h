//
// Created by johnk on 12/1/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class Device;
    struct DeviceCreateInfo;

    struct GpuProperty {
        uint32_t vendorId;
        uint32_t deviceId;
        GpuType type;
    };

    class Gpu {
    public:
        NON_COPYABLE(Gpu)
        virtual ~Gpu();
        virtual GpuProperty GetProperty() = 0;
        virtual Device* RequestDevice(const DeviceCreateInfo& createInfo) = 0;

    protected:
        Gpu();
    };
}
