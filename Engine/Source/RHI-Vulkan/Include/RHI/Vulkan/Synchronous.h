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
        explicit VKFence(VKDevice& device);
        ~VKFence() override;

        void Signal(uint32_t value) override;
        void Wait(uint32_t value) override;
        void Destroy() override;

        VkSemaphore GetTimelineSemaphore() const;

    private:
        VKDevice& device;
        VkSemaphore vkTimelineSemaphore;
        PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKhrFunc;
        PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKhrFunc;
    };
}
