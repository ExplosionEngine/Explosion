//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <cstdint>
#include <vector>

#include <Common/Utility.h>

namespace RHI {
    class CommandBuffer;
    class Fence;
    class Semaphore;

    struct QueueSubmitInfo {
        std::vector<Semaphore*> waitSemaphores;
        std::vector<Semaphore*> signalSemaphores;
        Fence* signalFence;

        QueueSubmitInfo();
        QueueSubmitInfo& AddWaitSemaphore(Semaphore* inSemaphore);
        QueueSubmitInfo& AddSignalSemaphore(Semaphore* inSemaphore);
        QueueSubmitInfo& SetSignalFence(Fence* inSignalFence);
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
