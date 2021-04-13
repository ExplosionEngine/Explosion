//
// Created by John Kindem on 2021/4/10.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enums.h>

namespace Explosion {
    class SwapChain;
    class Device;

    class Image {
    public:
        struct Info {
            ImageType type;
            Format format;
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mipLevels;
            uint32_t layers;
            ImageViewType viewType;
        };

        virtual ~Image();
        virtual const VkImage& GetVkImage() = 0;
        virtual const VkImageView& GetVkImageView() = 0;
        virtual const Info& GetInfo() = 0;

    protected:
        Image();
    };

    class SwapChainImage : public Image {
    public:
        SwapChainImage(Device& device, SwapChain& swapChain, uint32_t imageIndex);
        ~SwapChainImage() override;
        VkImage const& GetVkImage() override;
        VkImageView const& GetVkImageView() override;
        const Info& GetInfo() override;

    private:
        void FetchImage();
        void CreateImageView();
        void DestroyImageView();

        void WriteImageInfo();

        Device& device;
        SwapChain& swapChain;
        Info info;
        uint32_t imageIndex;
        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
    };

    class Attachment : public Image {
    public:
        struct Config {

        };

        Attachment(Device& device, Config config);
        ~Attachment() override;
        VkImage const& GetVkImage() override;
        VkImageView const& GetVkImageView() override;
        const Info& GetInfo() override;

    private:
        void CreateImage();
        void DestroyImage();

        void CreateImageView();
        void DestroyImageView();

        void WriteImageInfo();

        Device& device;
        Config config;
        Info info;
        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
    };

    class ExternalImage : public Image {
    public:
        struct Config {

        };

        ExternalImage(Device& device, Config config);
        ~ExternalImage() override;
        void Update(void* data);
        VkImage const& GetVkImage() override;
        VkImageView const& GetVkImageView() override;
        const Info& GetInfo() override;

    private:
        void CreateImage();
        void DestroyImage();

        void CreateImageView();
        void DestroyImageView();

        void WriteImageInfo();

        Device& device;
        Config config;
        Info info;
        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_IMAGE_H
