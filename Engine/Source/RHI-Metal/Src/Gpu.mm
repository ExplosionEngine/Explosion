//
// Created by Zach Lee on 2022/9/22.
//

#include <Metal/Gpu.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLGpu::MTLGpu(MTLInstance& inst) : instance(inst)
    {
    }

    MTLGpu::~MTLGpu()
    {
    }

    GpuProperty MTLGpu::GetProperty()
    {
        return property;
    }

    Device* MTLGpu::RequestDevice(const DeviceCreateInfo& createInfo)
    {
        return new MTLDevice(*this, createInfo);
    }

    id<MTLDevice> MTLGpu::GetDevice() const
    {
        return mtlDevice;
    }

    void MTLGpu::Init(id<MTLDevice> device)
    {
        mtlDevice = device;

        /**
         * GPUType    lowPower removable
         * Integrated Y        N
         * Discrete   N        N
         * External   N        Y
         */
        if (device.isRemovable) {
            property.type = GpuType::software;
        } else {
            property.type = GpuType::hardware;
        }
    }

}