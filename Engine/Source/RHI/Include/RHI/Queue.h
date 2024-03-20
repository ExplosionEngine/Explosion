//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>

namespace RHI {
    class CommandBuffer;
    class Fence;

    struct QueueSubmitInfo {
        Fence* waitFence;
        uint32_t waitFenceValue;
        Fence* signalFence;
        uint32_t signalFenceValue;
    };

    struct QueueFlushInfo {
        Fence* signalFence;
        uint32_t signalFenceValue;
    };

    class Queue {
    public:
        NonCopyable(Queue)
        virtual ~Queue();

        virtual void Submit(CommandBuffer* commandBuffer, const QueueSubmitInfo& submitInfo) = 0;
        virtual void Flush(const QueueFlushInfo& flushInfo) = 0;

    protected:
        Queue();
    };
}
