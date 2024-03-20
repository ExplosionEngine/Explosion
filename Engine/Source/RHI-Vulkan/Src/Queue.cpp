//
// Created by johnk on 16/1/2022.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    VKQueue::VKQueue(VkQueue q) : Queue(), vkQueue(q) {}

    VKQueue::~VKQueue() = default;

    void VKQueue::Submit(CommandBuffer* cb, const QueueSubmitInfo& submitInfo)
    {
        auto* commandBuffer = static_cast<VKCommandBuffer*>(cb);
        auto* fenceToSignaled = static_cast<VKFence*>(submitInfo.signalFence);
        Assert(commandBuffer);

        const VkCommandBuffer& cmdBuffer = commandBuffer->GetVkCommandBuffer();
        const VkFence& fence = fenceToSignaled == nullptr ? VK_NULL_HANDLE : fenceToSignaled->GetVkFence();

        VkSubmitInfo vkSubmitInfo = {};
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.commandBufferCount = 1;
        vkSubmitInfo.waitSemaphoreCount = 0;
        vkSubmitInfo.pWaitSemaphores = nullptr;
        vkSubmitInfo.pWaitDstStageMask = nullptr;
        vkSubmitInfo.signalSemaphoreCount = 0;
        vkSubmitInfo.pSignalSemaphores = nullptr;
        vkSubmitInfo.pCommandBuffers = &cmdBuffer;

        Assert(vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, fence) == VK_SUCCESS);
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
