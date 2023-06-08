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
        ~VKFence();

        FenceStatus GetStatus() override;
        void Reset() override;
        void Wait() override;
        void Destroy() override;

        VkFence GetVkFence();

    private:
        void CreateVKFence();

        VKDevice& device;
        VkFence fence;
        bool signaled;
    };
}
