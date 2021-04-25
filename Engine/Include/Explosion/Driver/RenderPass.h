//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_RENDERPASS_H
#define EXPLOSION_RENDERPASS_H

#include <vector>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Driver;
    class Device;
    class ImageView;

    class RenderPass {
    public:
        struct Config {
            uint32_t width;
            uint32_t height;
            uint32_t layers;
            std::vector<ImageView*> colorAttachments;
            ImageView* depthStencilAttachment;
        };

        explicit RenderPass(Driver& driver, const Config& config);
        ~RenderPass();

    private:
        void ValidateAttachments();

        void CreateRenderPass();
        void DestroyRenderPass();

        void CreateFrameBuffer();
        void DestroyFrameBuffer();

        Driver& driver;
        Device& device;
        Config config {};
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;
        VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_RENDERPASS_H
