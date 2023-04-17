//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Instance.h>
#include <RHI/Dummy/Api.h>

namespace RHI::Dummy {
    class DummyGpu;

    class DummyInstance : public Instance {
    public:
        NON_COPYABLE(DummyInstance)
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

extern "C" {
    RHI_DUMMY_API RHI::Instance* RHIGetInstance();
}
