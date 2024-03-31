//
// Created by swtpotato on 2022/8/2.
//

#pragma once

#include <vulkan/vulkan.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanFence : public Fence {
    public:
        NonCopyable(VulkanFence)
        explicit VulkanFence(VulkanDevice& inDevice, bool inInitAsSignaled);
        ~VulkanFence() override;

        bool IsSignaled() override;
        void Reset() override;
        void Wait() override;
        void Destroy() override;

        VkFence GetNative() const;

    private:
        VulkanDevice& device;
        VkFence nativeFence;
    };

    class VulkanSemaphore : public Semaphore {
    public:
        NonCopyable(VulkanSemaphore)
        explicit VulkanSemaphore(VulkanDevice& inDevice);
        ~VulkanSemaphore() override;

        void Destroy() override;

        VkSemaphore GetNative() const;

    private:
        VulkanDevice& device;
        VkSemaphore nativeSemaphore;
    };
}
