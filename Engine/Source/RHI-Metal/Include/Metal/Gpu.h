//
// Created by Zach Lee on 2022/9/22.
//

#pragma once

#include <RHI/Gpu.h>
#import <Metal/Metal.h>

namespace RHI::Metal {

    class MTLInstance;

    class MTLGpu : public Gpu {
    public:
        NON_COPYABLE(MTLGpu)

        explicit MTLGpu(MTLInstance& inst);
        ~MTLGpu() override;

        GpuProperty GetProperty() override;
        Device* RequestDevice(const DeviceCreateInfo& createInfo) override;

        id <MTLDevice> GetDevice() const;
        void Init(id<MTLDevice> device);

    private:
        MTLInstance& instance;
        id<MTLDevice> mtlDevice = nil;
        GpuProperty property;
    };


}
