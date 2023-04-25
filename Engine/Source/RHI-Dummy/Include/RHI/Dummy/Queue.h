//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Queue.h>

namespace RHI::Dummy {
    class DummyQueue : public Queue {
    public:
        NON_COPYABLE(DummyQueue)
        DummyQueue();
        ~DummyQueue() override;

        void Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal) override;
        void Wait(Fence* fenceToSignal) override;
    };
}
