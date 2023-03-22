//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Instance.h>
#include <RHI/Dummy/Gpu.h>
#include <Common/Debug.h>

namespace RHI::Dummy {
    DummyInstance::DummyInstance()
        : dummyGpu(std::make_unique<DummyGpu>())
    {
    }

    DummyInstance::~DummyInstance() = default;

    RHIType DummyInstance::GetRHIType()
    {
        return RHIType::DUMMY;
    }

    uint32_t DummyInstance::GetGpuNum()
    {
        return 1;
    }

    Gpu* DummyInstance::GetGpu(uint32_t index)
    {
        Assert(index == 0);
        return dummyGpu.get();
    }

    void DummyInstance::Destroy()
    {
        delete this;
    }
}

extern "C" {
    RHI::Instance* RHIGetInstance()
    {
        static RHI::Dummy::DummyInstance singleton;
        return &singleton;
    }
}
