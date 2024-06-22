//
// Created by johnk on 16/1/2022.
//

#include <vector>

#include <Common/Debug.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    VulkanQueue::VulkanQueue(VulkanDevice&, const VkQueue inNativeQueue)
        : nativeQueue(inNativeQueue)
    {
    }

    VulkanQueue::~VulkanQueue() = default;

    void VulkanQueue::Submit(CommandBuffer* inCmdBuffer, const QueueSubmitInfo& inSubmitInfo)
    {
        const auto* commandBuffer = static_cast<VulkanCommandBuffer*>(inCmdBuffer);
        const auto* vkFence = static_cast<VulkanFence*>(inSubmitInfo.signalFence);

        Assert(commandBuffer);
        const VkCommandBuffer& cmdBuffer = commandBuffer->GetNativeCommandBuffer();

        std::vector<VkSemaphore> waitSemaphores;
        std::vector<VkPipelineStageFlags> waitStageFlags;
        waitSemaphores.resize(inSubmitInfo.waitSemaphores.size());
        waitStageFlags.resize(inSubmitInfo.waitSemaphores.size());
        for (auto i = 0; i < inSubmitInfo.waitSemaphores.size(); i++) {
            const auto* vkSemaphore = static_cast<VulkanSemaphore*>(inSubmitInfo.waitSemaphores[i]);
            waitSemaphores[i] = vkSemaphore->GetNative();
            waitStageFlags[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }

        std::vector<VkSemaphore> signalSemaphores;
        signalSemaphores.resize(inSubmitInfo.signalSemaphores.size());
        for (auto i = 0; i < inSubmitInfo.signalSemaphores.size(); i++) {
            const auto* vkSemaphore = static_cast<VulkanSemaphore*>(inSubmitInfo.signalSemaphores[i]);
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

        const VkFence nativeFence = vkFence == nullptr ? VK_NULL_HANDLE : vkFence->GetNative();
        Assert(vkQueueSubmit(nativeQueue, 1, &vkSubmitInfo, nativeFence) == VK_SUCCESS);
    }

    void VulkanQueue::Flush(Fence* inFenceToSignal)
    {
        const auto* vkFence = static_cast<VulkanFence*>(inFenceToSignal);

        VkSubmitInfo vkSubmitInfo = {};
        vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo.commandBufferCount = 0;
        vkSubmitInfo.pCommandBuffers = nullptr;

        Assert(vkQueueSubmit(nativeQueue, 1, &vkSubmitInfo, vkFence->GetNative()) == VK_SUCCESS);
    }

    VkQueue VulkanQueue::GetNative() const
    {
        return nativeQueue;
    }
}
