//
// Created by John Kindem on 2021/4/25.
//

#ifndef EXPLOSION_FRAMEBUFFER_H
#define EXPLOSION_FRAMEBUFFER_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/GpuRes.h>

namespace Explosion {
    class Driver;
    class Device;
    class ImageView;
    class RenderPass;

    class FrameBuffer : public GpuRes {
    public:
        struct Config {
            RenderPass* renderPass;
            uint32_t width;
            uint32_t height;
            uint32_t layers;
            std::vector<ImageView*> attachments;
        };

        FrameBuffer(Driver& driver, const Config& config);
        ~FrameBuffer() override;
        const VkFramebuffer& GetVkFrameBuffer() const;

    protected:
        void OnCreate() override;
        void OnDestroy() override;

    private:
        void CreateFrameBuffer();
        void DestroyFrameBuffer();

        Device& device;
        Config config {};
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_FRAMEBUFFER_H
