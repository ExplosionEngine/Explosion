//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
    class Driver;
    class Device;

    class Image {
    public:
        struct Config {
            ImageType imageType;
            Format format;
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mipLevels;
            uint32_t layers;
        };

        Image(Driver& driver, const Config& config);
        explicit Image(Driver& driver, const VkImage& vkImage, const Config& config);
        ~Image();

        const VkImage& GetVkImage();
        const Config& GetConfig();

    protected:
        Driver& driver;
        Device& device;

        virtual void OnSetupImageCreateInfo(VkImageCreateInfo& createInfo);

    private:
        void CreateImage();
        void DestroyImage();

        bool fromSwapChain = false;
        VkImage vkImage = VK_NULL_HANDLE;
        Config config {};
    };

    class ColorAttachment : public Image {
    public:
        ColorAttachment(Driver& driver, const Config& config);
        ColorAttachment(Driver& driver, const VkImage& vkImage, const Config& config);
        ~ColorAttachment();
        bool IsFromSwapChain();

    protected:
        void OnSetupImageCreateInfo(VkImageCreateInfo &createInfo) override;

    private:
        bool fromSwapChain;
    };

    // TODO
    class DepthStencilAttachment : public Image {
    public:

    private:
    };

    // TODO
    class ExternalImage : public Image {
    public:

    private:
    };
}

#endif //EXPLOSION_IMAGE_H
