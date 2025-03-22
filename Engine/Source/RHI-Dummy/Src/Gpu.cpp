//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Gpu.h>
#include <RHI/Dummy/Device.h>

namespace RHI::Dummy {
    DummyGpu::DummyGpu(DummyInstance& inInstance)
        : instance(inInstance)
    {
    }

    DummyGpu::~DummyGpu() = default;

    GpuProperty DummyGpu::GetProperty()
    {
        return {};
    }

    Common::UniquePtr<Device> DummyGpu::RequestDevice(const DeviceCreateInfo& createInfo)
    {
        return { new DummyDevice(*this, createInfo) };
    }

    DummyInstance& DummyGpu::GetInstance() const
    {
        return instance;
    }
}
