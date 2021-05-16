//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_VULKANRENDERPASS_H
#define EXPLOSION_VULKANRENDERPASS_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanImageView;

    class VulkanRenderPass {
    public:
        struct AttachmentConfig {
            AttachmentType type;
            Format format;
            AttachmentLoadOp loadOp;
            AttachmentStoreOp storeOp;
        };

        struct Config {
            std::vector<AttachmentConfig> attachmentConfigs;
        };

        explicit VulkanRenderPass(VulkanDriver& driver, Config config);
        ~VulkanRenderPass();
        const VkRenderPass& GetVkRenderPass();

    private:
        void CreateRenderPass();
        void DestroyRenderPass();

        VulkanDriver& driver;
        VulkanDevice& device;
        Config config {};
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANRENDERPASS_H
