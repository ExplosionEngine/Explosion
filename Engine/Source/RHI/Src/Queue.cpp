//
// Created by johnk on 15/1/2022.
//

#include <RHI/Queue.h>

namespace RHI {
    QueueSubmitInfo::QueueSubmitInfo()
        : signalFence(nullptr)
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

    QueueSubmitInfo& QueueSubmitInfo::SetWaitSemaphores(const std::vector<Semaphore*>& inSemaphores)
    {
        waitSemaphores = inSemaphores;
        return *this;
    }

    QueueSubmitInfo& QueueSubmitInfo::SetSignalSemaphores(const std::vector<Semaphore*>& inSemaphores)
    {
        signalSemaphores = inSemaphores;
        return *this;
    }

    Queue::Queue() = default;

    Queue::~Queue() = default;
}
