//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_VULKANRENDERPASS_H
#define EXPLOSION_VULKANRENDERPASS_H

#include <vulkan/vulkan.h>

#include <RHI/RenderPass.h>
#include <RHI/Vulkan/Api.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanImageView;

    class VulkanRenderPass : public RenderPass {
    public:
        explicit VulkanRenderPass(VulkanDriver& driver, Config config);
        ~VulkanRenderPass() override;
        const VkRenderPass& GetVkRenderPass();

    private:
        void CreateRenderPass();
        void DestroyRenderPass();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANRENDERPASS_H
