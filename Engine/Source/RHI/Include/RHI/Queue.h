//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>

namespace RHI {
    class CommandBuffer;
    class Fence;
    class Semaphore;

    struct QueueSubmitInfo {
        uint8_t waitSemaphoreNum;
        Semaphore* waitSemaphores;
        uint8_t signalSemaphoreNum;
        Semaphore* signalSemaphores;
        Fence* signalFence;
    };

    class Queue {
    public:
        NonCopyable(Queue)
        virtual ~Queue();

        virtual void Submit(CommandBuffer* commandBuffer, const QueueSubmitInfo& submitInfo) = 0;
        virtual void Flush(Fence* fenceToSignal) = 0;

    protected:
        Queue();
    };
}
