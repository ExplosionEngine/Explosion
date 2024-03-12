//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Instance.h>
#include <RHI/Dummy/Gpu.h>
#include <Common/Debug.h>

namespace RHI::Dummy {
    RHI::Instance* gInstance = nullptr;

    DummyInstance::DummyInstance()
        : dummyGpu(Common::MakeUnique<DummyGpu>())
    {
    }

    DummyInstance::~DummyInstance() = default;

    RHIType DummyInstance::GetRHIType()
    {
        return RHIType::dummy;
    }

    uint32_t DummyInstance::GetGpuNum()
    {
        return 1;
    }

    Gpu* DummyInstance::GetGpu(uint32_t index)
    {
        Assert(index == 0);
        return dummyGpu.Get();
    }

    void DummyInstance::Destroy()
    {
        delete this;
    }
}
