//
// Created by Zach Lee on 2022/9/22.
//

#include <Metal/Instance.h>

namespace RHI::Metal {


    MTLInstance::MTLInstance()
    {
        @autoreleasepool
        {
            NSArray<id<MTLDevice>> *devices = MTLCopyAllDevices();
            for (id device in devices) {
                auto gpu = new MTLGpu(*this);
                gpu->Init(device);
                gpus.emplace_back(gpu);
            }
        }
    }

    MTLInstance::~MTLInstance()
    {

    }

    RHIType MTLInstance::GetRHIType()
    {
        return RHIType::METAL;
    }

    uint32_t MTLInstance::GetGpuNum()
    {
        return static_cast<uint32_t>(gpus.size());
    }

    Gpu* MTLInstance::GetGpu(uint32_t index)
    {
        return gpus[index].Get();
    }

    void MTLInstance::Destroy()
    {
        delete this;
    }
}

extern "C" {
    RHI::Instance* RHIGetInstance()
    {
        static RHI::Metal::MTLInstance instance;
        return &instance;
    }
}