//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_RENDERPASS_H
#define EXPLOSION_RENDERPASS_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
    class Driver;
    class Device;
    class ImageView;

    class RenderPass {
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

        explicit RenderPass(Driver& driver, const Config& config);
        ~RenderPass();
        const VkRenderPass& GetVkRenderPass();

    private:
        void CreateRenderPass();
        void DestroyRenderPass();

        Driver& driver;
        Device& device;
        Config config {};
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_RENDERPASS_H
