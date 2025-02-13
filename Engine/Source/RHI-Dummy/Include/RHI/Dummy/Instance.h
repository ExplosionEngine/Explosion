//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Instance.h>

namespace RHI::Dummy {
    class DummyGpu;

    extern Instance* gInstance;

    class DummyInstance final : public Instance {
    public:
        NonCopyable(DummyInstance)
        DummyInstance();
        ~DummyInstance() override;
        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;
        void Destroy() override;

    private:
        Common::UniquePtr<DummyGpu> dummyGpu;
    };
}
