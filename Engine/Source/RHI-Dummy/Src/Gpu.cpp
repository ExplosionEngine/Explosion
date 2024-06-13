//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Gpu.h>
#include <RHI/Dummy/Device.h>

namespace RHI::Dummy {
    DummyGpu::DummyGpu() = default;

    DummyGpu::~DummyGpu() = default;

    GpuProperty DummyGpu::GetProperty()
    {
        return {};
    }

    Common::UniqueRef<Device> DummyGpu::RequestDevice(const DeviceCreateInfo& createInfo)
    {
        return { new DummyDevice(createInfo) };
    }
}
