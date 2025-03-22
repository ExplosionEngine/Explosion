//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Gpu.h>
#include <RHI/Dummy/Instance.h>

namespace RHI::Dummy {
    class DummyGpu final : public Gpu {
    public:
        NonCopyable(DummyGpu)
        explicit DummyGpu(DummyInstance& inInstance);
        ~DummyGpu() override;
        GpuProperty GetProperty() override;
        Common::UniquePtr<Device> RequestDevice(const DeviceCreateInfo& createInfo) override;
        DummyInstance& GetInstance() const override;

    private:
        DummyInstance& instance;
    };
}
