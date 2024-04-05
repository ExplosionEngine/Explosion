//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <RHI/CommandBuffer.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanCommandBuffer : public CommandBuffer {
    public:
        NonCopyable(VulkanCommandBuffer)
        VulkanCommandBuffer(VulkanDevice& inDevice, VkCommandPool inNativeCmdPool);
        ~VulkanCommandBuffer() override;

        void Destroy() override;
        CommandRecorder* Begin() override;

        VkCommandBuffer GetNativeCommandBuffer() const;

    private:
        void CreateNativeCommandBuffer();

        VulkanDevice& device;
        VkCommandPool pool;
        VkCommandBuffer nativeCmdBuffer;
    };

}
