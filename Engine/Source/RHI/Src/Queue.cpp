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

    QueueSubmitInfo& QueueSubmitInfo::AddWaitSemaphore(Semaphore* inSemaphore)
    {
        waitSemaphores.emplace_back(inSemaphore);
        return *this;
    }

    QueueSubmitInfo& QueueSubmitInfo::AddSignalSemaphore(Semaphore* inSemaphore)
    {
        signalSemaphores.emplace_back(inSemaphore);
        return *this;
    }

    QueueSubmitInfo& QueueSubmitInfo::SetSignalFence(Fence* inSignalFence)
    {
        signalFence = inSignalFence;
        return *this;
    }

    Queue::Queue() = default;

    Queue::~Queue() = default;
}
