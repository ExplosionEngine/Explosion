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

    void VKQueue::Submit(CommandBuffer* cb, Fence* fenceToSignal)
    {
        auto* commandBuffer = static_cast<VKCommandBuffer*>(cb);
        auto* vkFence = static_cast<VKFence*>(fenceToSignal);

        Assert(commandBuffer);
        const VkCommandBuffer& cmdBuffer = commandBuffer->GetVkCommandBuffer();

        VkSubmitInfo vkSubmitInfo = {};
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.waitSemaphoreCount = 0;
        vkSubmitInfo.pWaitSemaphores = nullptr;
        vkSubmitInfo.signalSemaphoreCount = 0;
        vkSubmitInfo.pSignalSemaphores = nullptr;
        vkSubmitInfo.commandBufferCount = 1;
        vkSubmitInfo.pCommandBuffers = &cmdBuffer;

        Assert(vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, vkFence->GetNative()) == VK_SUCCESS);
    }

    void VKQueue::Flush(Fence* fenceToSignal)
    {
        // TODO
    }

    VkQueue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
