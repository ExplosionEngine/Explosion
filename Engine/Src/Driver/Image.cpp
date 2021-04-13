//
// Created by John Kindem on 2021/4/10.
//

#include <Explosion/Driver/Image.h>

namespace Explosion {
    Image::Image() = default;

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
        // TODO
    }

    void SwapChainImage::CreateImageView()
    {
        // TODO
    }

    void SwapChainImage::DestroyImageView()
    {
        // TODO
    }

    void SwapChainImage::WriteImageInfo()
    {
        // TODO
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
