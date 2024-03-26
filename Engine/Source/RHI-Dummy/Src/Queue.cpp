//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Queue.h>

namespace RHI::Dummy {
    DummyQueue::DummyQueue() = default;

    DummyQueue::~DummyQueue() = default;

    void DummyQueue::Submit(RHI::CommandBuffer* commandBuffer, RHI::Fence* fenceToSignal)
    {
    }

    void DummyQueue::Flush(RHI::Fence* fenceToSignal)
    {
    }
}
