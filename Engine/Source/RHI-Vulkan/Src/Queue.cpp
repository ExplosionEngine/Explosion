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

    void VKQueue::Submit(CommandBuffer* cb, Fence* fts)
    {
        auto* commandBuffer = dynamic_cast<VKCommandBuffer*>(cb);
        auto* fenceToSignaled = dynamic_cast<VKFence*>(fts);
        Assert(commandBuffer);

        const VkCommandBuffer& cmdBuffer = commandBuffer->GetVkCommandBuffer();
        const VkFence& fence = fenceToSignaled == nullptr ? VK_NULL_HANDLE : fenceToSignaled->GetVkFence();

        VkSubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pWaitSemaphores = commandBuffer->GetWaitSemaphores().data();
        submitInfo.pWaitDstStageMask = commandBuffer->GetWaitStages().data();
        submitInfo.pSignalSemaphores = commandBuffer->GetSignalSemaphores().data();
        submitInfo.pCommandBuffers = &cmdBuffer;

        if (fenceToSignaled != nullptr) {
            fenceToSignaled->Reset();
        }
        Assert(vkQueueSubmit(vkQueue, 1, &submitInfo, fence) == VK_SUCCESS);
    }

    void VKQueue::Wait(Fence* fenceToSignal)
    {
        // TODO
    }

    VkQueue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
