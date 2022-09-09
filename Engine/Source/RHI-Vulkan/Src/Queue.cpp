//
// Created by johnk on 16/1/2022.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    VKQueue::VKQueue(vk::Queue q) : Queue(), vkQueue(q) {}

    VKQueue::~VKQueue() = default;

    void VKQueue::Submit(CommandBuffer* cb, Fence* fts)
    {
        auto* commandBuffer = dynamic_cast<VKCommandBuffer*>(cb);
        auto* fenceToSignaled = dynamic_cast<VKFence*>(fts);
        Assert(commandBuffer && fenceToSignaled);

        const vk::CommandBuffer& vcb = commandBuffer->GetVkCommandBuffer();
        const vk::Fence& fence = fenceToSignaled->GetVkFence();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBufferCount(1)
            .setWaitSemaphores(commandBuffer->GetWaitSemaphores())
            .setWaitDstStageMask(commandBuffer->GetWaitStages())
            .setSignalSemaphores(commandBuffer->GetSignalSemaphores())
            .setPCommandBuffers(&vcb);

        fenceToSignaled->Reset();
        Assert(vkQueue.submit(1, &submitInfo, fence) == vk::Result::eSuccess);
    }

    void VKQueue::Wait(Fence* fenceToSignal)
    {
        // TODO
    }

    vk::Queue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
