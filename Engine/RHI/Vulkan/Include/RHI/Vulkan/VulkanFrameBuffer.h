//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_VULKANFRAMEBUFFER_H
#define EXPLOSION_VULKANFRAMEBUFFER_H

#include <vector>

#include <vulkan/vulkan.h>

#include <RHI/FrameBuffer.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanImageView;
    class VulkanRenderPass;

    class VulkanFrameBuffer : public FrameBuffer {
    public:
        VulkanFrameBuffer(VulkanDriver& driver, Config config);
        ~VulkanFrameBuffer() override;
        const VkFramebuffer& GetVkFrameBuffer();

    private:
        void CreateFrameBuffer();
        void DestroyFrameBuffer();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANFRAMEBUFFER_H
