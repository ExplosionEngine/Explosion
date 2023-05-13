//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Synchronous.h>
#include <RHI/Dummy/Device.h>

namespace RHI::Dummy {
    DummyFence::DummyFence(DummyDevice& device) : Fence(device)
    {
    }

    DummyFence::~DummyFence() = default;

    FenceStatus DummyFence::GetStatus()
    {
        return FenceStatus::signaled;
    }

    void DummyFence::Reset()
    {
    }

    void DummyFence::Wait()
    {
    }

    void DummyFence::Destroy()
    {
        delete this;
    }
}
