//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Queue.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanQueue final : public Queue {
    public:
        NonCopyable(VulkanQueue)
        explicit VulkanQueue(VulkanDevice& inDevice, VkQueue inNativeQueue);
        ~VulkanQueue() override;

        void Submit(CommandBuffer* inCmdBuffer, const QueueSubmitInfo& inSubmitInfo) override;
        void Flush(Fence* inFenceToSignal) override;

        VkQueue GetNative() const;

    private:
        VkQueue nativeQueue;
    };
}
