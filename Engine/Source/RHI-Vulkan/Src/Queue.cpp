//
// Created by johnk on 16/1/2022.
//

#include <vector>

#include <Common/Debug.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    VKQueue::VKQueue(VKDevice& inDevice, VkQueue q)
        : Queue()
        , vkQueue(q)
    {
    }

    VKQueue::~VKQueue() = default;

    void VKQueue::Submit(CommandBuffer* cb, const QueueSubmitInfo& submitInfo)
    {
        auto* commandBuffer = static_cast<VKCommandBuffer*>(cb);
        auto* fenceToWait = static_cast<VKFence*>(submitInfo.waitFence);
        auto* fenceToSignal = static_cast<VKFence*>(submitInfo.signalFence);

        Assert(commandBuffer);
        const VkCommandBuffer& cmdBuffer = commandBuffer->GetVkCommandBuffer();

        const bool hasFenceToWait = fenceToWait != nullptr;
        const bool hasFenceToSignal = fenceToSignal != nullptr;
        const bool hasAnyFence = hasFenceToWait || hasFenceToSignal;

        std::vector<VkSemaphore> waitTimelineSemaphores;
        std::vector<VkSemaphore> signalTimelineSemaphores;
        std::vector<uint64_t> waitValues;
        std::vector<uint64_t> signalValues;

        if (hasFenceToWait) {
            waitTimelineSemaphores.emplace_back(fenceToWait->GetTimelineSemaphore());
            waitValues.emplace_back(submitInfo.waitFenceValue);
        }
        if (hasFenceToSignal) {
            signalTimelineSemaphores.emplace_back(fenceToSignal->GetTimelineSemaphore());
            signalValues.emplace_back(submitInfo.signalFenceValue);
        }

        VkSubmitInfo vkSubmitInfo = {};
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.waitSemaphoreCount = 0;
        vkSubmitInfo.pWaitSemaphores = nullptr;
        vkSubmitInfo.signalSemaphoreCount = 0;
        vkSubmitInfo.pSignalSemaphores = nullptr;
        vkSubmitInfo.commandBufferCount = 1;
        vkSubmitInfo.pCommandBuffers = &cmdBuffer;

        VkTimelineSemaphoreSubmitInfoKHR timelineSemaphoreSubmitInfo {};
        timelineSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR;

        if (hasFenceToWait) {
            vkSubmitInfo.waitSemaphoreCount = waitTimelineSemaphores.size();
            vkSubmitInfo.pWaitSemaphores = waitTimelineSemaphores.data();
            timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = waitValues.size();
            timelineSemaphoreSubmitInfo.pWaitSemaphoreValues = waitValues.data();
        }
        if (hasFenceToSignal) {
            vkSubmitInfo.signalSemaphoreCount = signalTimelineSemaphores.size();
            vkSubmitInfo.pSignalSemaphores = signalTimelineSemaphores.data();
            timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = signalValues.size();
            timelineSemaphoreSubmitInfo.pSignalSemaphoreValues = signalValues.data();
        }
        if (hasAnyFence) {
            vkSubmitInfo.pNext = &timelineSemaphoreSubmitInfo;
        }

        Assert(vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE) == VK_SUCCESS);
    }

    void VKQueue::Flush(const QueueFlushInfo& flushInfo)
    {
        // TODO
    }

    VkQueue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
