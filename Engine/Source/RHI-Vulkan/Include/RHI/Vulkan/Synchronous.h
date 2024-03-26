//
// Created by swtpotato on 2022/8/2.
//

#pragma once

#include <vulkan/vulkan.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKFence : public Fence {
    public:
        explicit VKFence(VKDevice& inDevice, bool initAsSignaled);
        ~VKFence() override;

        bool IsSignaled() override;
        void Reset() override;
        void Wait() override;
        void Destroy() override;

        VkFence GetNative() const;

    private:
        VKDevice& device;
        VkFence vkFence;
    };

    class VKSemaphore : public Semaphore {
    public:
        explicit VKSemaphore(VKDevice& inDevice);
        ~VKSemaphore() override;

        void Destroy() override;

        VkSemaphore GetNative() const;

    private:
        VKDevice& device;
        VkSemaphore vkSemaphore;
    };
}
