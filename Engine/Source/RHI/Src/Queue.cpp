//
// Created by johnk on 15/1/2022.
//

#include <RHI/Queue.h>

namespace RHI {
    QueueSubmitInfo::QueueSubmitInfo()
        : waitSemaphores()
        , signalSemaphores()
        , signalFence(nullptr)
    {
    }

    QueueSubmitInfo& QueueSubmitInfo::WaitSemaphore(Semaphore* inSemaphore)
    {
        waitSemaphores.emplace_back(inSemaphore);
        return *this;
    }

    QueueSubmitInfo& QueueSubmitInfo::SignalSemaphore(Semaphore* inSemaphore)
    {
        signalSemaphores.emplace_back(inSemaphore);
        return *this;
    }

    QueueSubmitInfo& QueueSubmitInfo::SignalFence(Fence* inSignalFence)
    {
        signalFence = inSignalFence;
        return *this;
    }

    Queue::Queue() = default;

    Queue::~Queue() = default;
}
