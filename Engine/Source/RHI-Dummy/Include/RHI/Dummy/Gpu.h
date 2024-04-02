//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Gpu.h>

namespace RHI::Dummy {
    class DummyGpu : public Gpu {
    public:
        NonCopyable(DummyGpu)
        DummyGpu();
        ~DummyGpu() override;
        GpuProperty GetProperty() override;
        Common::UniqueRef<Device> RequestDevice(const DeviceCreateInfo& createInfo) override;
    };
}
