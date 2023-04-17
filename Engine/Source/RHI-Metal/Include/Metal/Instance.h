//
// Created by Zach Lee on 2022/9/22.
//

#pragma once

#include <vector>
#include <RHI/Instance.h>
#include <Metal/Gpu.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLInstance : public Instance {
    public:
        NON_COPYABLE(MTLInstance)

        MTLInstance();
        ~MTLInstance() override;

        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;
        void Destroy() override;

    private:
        std::vector<Common::UniqueRef<MTLGpu>> gpus;
    };
}