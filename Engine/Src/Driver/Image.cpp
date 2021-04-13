//
// Created by John Kindem on 2021/4/10.
//

#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/SwapChain.h>

namespace Explosion {
    Image::Image() = default;

    Image::~Image() = default;

    SwapChainImage::SwapChainImage(Device& device, SwapChain& swapChain, uint32_t imageIndex)
        : device(device), swapChain(swapChain), imageIndex(imageIndex)
    {
        FetchImage();
        CreateImageView();
        WriteImageInfo();
    }

    SwapChainImage::~SwapChainImage()
    {
        DestroyImageView();
    }

    VkImage const& SwapChainImage::GetVkImage()
    {
        return vkImage;
    }

    VkImageView const& SwapChainImage::GetVkImageView()
    {
        return vkImageView;
    }

    const Image::Info& SwapChainImage::GetInfo()
    {
        return info;
    }

    void SwapChainImage::FetchImage()
    {
        vkImage = swapChain.GetVkImages()[imageIndex];
    }

    void SwapChainImage::CreateImageView()
    {
        VkImageViewCreateInfo imageViewCreateInfo {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vkImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = swapChain.GetVkSurfaceFormat().format;
        imageViewCreateInfo.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
        imageViewCreateInfo.subresourceRange = {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        };

        if (vkCreateImageView(device.GetVkDevice(), &imageViewCreateInfo, nullptr, &vkImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view for swap chain image");
        }
    }

    void SwapChainImage::DestroyImageView()
    {
        vkDestroyImageView(device.GetVkDevice(), vkImageView, nullptr);
    }

    void SwapChainImage::WriteImageInfo()
    {
        info.type = ImageType::I2D;
        info.viewType = ImageViewType::IV2D;
        info.width = swapChain.GetVkExtent().width;
        info.height = swapChain.GetVkExtent().height;
        info.depth = 1;
        info.mipLevels = 1;
        info.layers = 1;
    }

    Attachment::Attachment(Device& device, Attachment::Config config)
        : device(device), config(config)
    {
        CreateImage();
        CreateImageView();
        WriteImageInfo();
    }

    Attachment::~Attachment()
    {
        DestroyImageView();
        DestroyImage();
    }

    VkImage const& Attachment::GetVkImage()
    {
        return vkImage;
    }

    VkImageView const& Attachment::GetVkImageView()
    {
        return vkImageView;
    }

    const Image::Info& Attachment::GetInfo()
    {
        return info;
    }

    void Attachment::CreateImage()
    {
        // TODO
    }

    void Attachment::DestroyImage()
    {
        // TODO
    }

    void Attachment::CreateImageView()
    {
        // TODO
    }

    void Attachment::DestroyImageView()
    {
        // TODO
    }

    void Attachment::WriteImageInfo()
    {
        // TODO
    }

    ExternalImage::ExternalImage(Device& device, ExternalImage::Config config)
        : device(device), config(config)
    {
        CreateImage();
        CreateImageView();
        WriteImageInfo();
    }

    ExternalImage::~ExternalImage()
    {
        DestroyImageView();
        DestroyImage();
    }

    void ExternalImage::Update(void* data)
    {
        // TODO
    }

    VkImage const& ExternalImage::GetVkImage()
    {
        return vkImage;
    }

    VkImageView const& ExternalImage::GetVkImageView()
    {
        return vkImageView;
    }

    const Image::Info& ExternalImage::GetInfo()
    {
        return info;
    }

    void ExternalImage::CreateImage()
    {
        // TODO
    }

    void ExternalImage::DestroyImage()
    {
        // TODO
    }

    void ExternalImage::CreateImageView()
    {
        // TODO
    }

    void ExternalImage::DestroyImageView()
    {
        // TODO
    }

    void ExternalImage::WriteImageInfo()
    {
        // TODO
    }
}
