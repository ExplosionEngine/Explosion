//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Queue.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKQueue : public Queue {
    public:
        NonCopyable(VKQueue)
        explicit VKQueue(VKDevice& inDevice, VkQueue vkQueue);
        ~VKQueue() override;

        void Submit(CommandBuffer* commandBuffer, const QueueSubmitInfo& submitInfo) override;
        void Flush(Fence* fenceToSignal) override;

        VkQueue GetVkQueue();

    private:
        VkQueue vkQueue;
    };
}
