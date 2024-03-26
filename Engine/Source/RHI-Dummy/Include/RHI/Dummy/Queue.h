//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Queue.h>

namespace RHI::Dummy {
    class DummyQueue : public Queue {
    public:
        NonCopyable(DummyQueue)
        DummyQueue();
        ~DummyQueue() override;

        void Submit(RHI::CommandBuffer* commandBuffer, RHI::Fence* fenceToSignal) override;
        void Flush(RHI::Fence* fenceToSignal) override;
    };
}
