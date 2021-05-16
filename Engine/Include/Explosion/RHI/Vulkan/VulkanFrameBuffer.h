//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_VULKANFRAMEBUFFER_H
#define EXPLOSION_VULKANFRAMEBUFFER_H

#include <vector>

#include <vulkan/vulkan.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanImageView;
    class VulkanRenderPass;

    class VulkanFrameBuffer {
    public:
        struct Config {
            VulkanRenderPass* renderPass;
            uint32_t width;
            uint32_t height;
            uint32_t layers;
            std::vector<VulkanImageView*> attachments;
        };

        VulkanFrameBuffer(VulkanDriver& driver, Config config);
        ~VulkanFrameBuffer();
        const VkFramebuffer& GetVkFrameBuffer();

    private:
        void CreateFrameBuffer();
        void DestroyFrameBuffer();

        VulkanDriver& driver;
        VulkanDevice& device;
        Config config {};
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANFRAMEBUFFER_H
