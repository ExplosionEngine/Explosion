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
        auto* vkFence = static_cast<VKFence*>(submitInfo.signalFence);

        Assert(commandBuffer);
        const VkCommandBuffer& cmdBuffer = commandBuffer->GetVkCommandBuffer();

        std::vector<VkSemaphore> waitSemaphores;
        std::vector<VkPipelineStageFlags> waitStageFlags;
        waitSemaphores.resize(submitInfo.waitSemaphores.size());
        waitStageFlags.resize(submitInfo.waitSemaphores.size());
        for (auto i = 0; i < submitInfo.waitSemaphores.size(); i++) {
            auto* vkSemaphore = static_cast<VKSemaphore*>(submitInfo.waitSemaphores[i]);
            waitSemaphores[i] = vkSemaphore->GetNative();
            waitStageFlags[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }

        std::vector<VkSemaphore> signalSemaphores;
        signalSemaphores.resize(submitInfo.signalSemaphores.size());
        for (auto i = 0; i < submitInfo.signalSemaphores.size(); i++) {
            auto* vkSemaphore = static_cast<VKSemaphore*>(submitInfo.signalSemaphores[i]);
            signalSemaphores[i] = vkSemaphore->GetNative();
        }

        VkSubmitInfo vkSubmitInfo = {};
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.waitSemaphoreCount = waitSemaphores.size();
        vkSubmitInfo.pWaitSemaphores = waitSemaphores.data();
        // TODO maybe we need expose this, but dx12 have no this param
        vkSubmitInfo.pWaitDstStageMask = waitStageFlags.data();
        vkSubmitInfo.signalSemaphoreCount = signalSemaphores.size();
        vkSubmitInfo.pSignalSemaphores = signalSemaphores.data();
        vkSubmitInfo.commandBufferCount = 1;
        vkSubmitInfo.pCommandBuffers = &cmdBuffer;

        Assert(vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, vkFence->GetNative()) == VK_SUCCESS);
    }

    void VKQueue::Flush(Fence* fenceToSignal)
    {
        auto* vkFence = static_cast<VKFence*>(fenceToSignal);

        VkSubmitInfo vkSubmitInfo = {};
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.commandBufferCount = 0;
        vkSubmitInfo.pCommandBuffers = nullptr;

        Assert(vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, vkFence->GetNative()) == VK_SUCCESS);
    }

    VkQueue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
