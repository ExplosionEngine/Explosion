//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Synchronous.h>
#include <RHI/Dummy/Device.h>

namespace RHI::Dummy {
    DummyFence::DummyFence(DummyDevice& device, const bool bInitAsSignal)
        : Fence(device, bInitAsSignal)
    {
    }

    DummyFence::~DummyFence() = default;

    bool DummyFence::IsSignaled()
    {
        return false;
    }

    void DummyFence::Reset()
    {
    }

    void DummyFence::Wait()
    {
    }

    DummySemaphore::DummySemaphore(DummyDevice& device)
        : Semaphore(device)
    {
    }

    DummySemaphore::~DummySemaphore() = default;
}
