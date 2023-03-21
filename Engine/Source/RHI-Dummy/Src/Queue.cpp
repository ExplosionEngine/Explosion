//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Queue.h>

namespace RHI::Dummy {
    DummyQueue::DummyQueue() = default;

    DummyQueue::~DummyQueue() = default;

    void DummyQueue::Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal)
    {
    }

    void DummyQueue::Wait(Fence* fenceToSignal)
    {
    }
}
