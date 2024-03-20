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

        void Submit(CommandBuffer* commandBuffer, const QueueSubmitInfo& submitInfo) override;
        void Flush(const RHI::QueueFlushInfo& flushInfo) override;
    };
}
