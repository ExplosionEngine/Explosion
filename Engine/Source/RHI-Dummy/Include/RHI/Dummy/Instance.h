//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Instance.h>
#include <RHI/Dummy/Api.h>

namespace RHI::Dummy {
    class DummyGpu;

    RHI::Instance* RHIGetInstance();

    class DummyInstance : public Instance {
    public:
        NonCopyable(DummyInstance)
        DummyInstance();
        ~DummyInstance() override;
        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;
        void Destroy() override;

    private:
        Common::UniqueRef<DummyGpu> dummyGpu;
    };
}
