//
// Created by John Kindem on 2021/4/24.
//

#include <Explosion/Driver/ImageView.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/EnumAdapter.h>

namespace {
    VkImageAspectFlags GetImageAspect(Explosion::Image* image)
    {
        if (dynamic_cast<Explosion::ColorAttachment*>(image) != nullptr) {
            return VK_IMAGE_ASPECT_COLOR_BIT;
        } else if (dynamic_cast<Explosion::DepthStencilAttachment*>(image) != nullptr) {
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        } else {
            return 0;
        }
    }
}

namespace Explosion {
    ImageView::ImageView(Driver& driver, Image* image, const ImageView::Config& config)
        : driver(driver), device(*driver.GetDevice()), image(image), config(config)
    {
        CreateImageView();
    }

    ImageView::~ImageView()
    {
        DestroyImageView();
    }

    Image* ImageView::GetImage()
    {
        return image;
    }

    const VkImageView& ImageView::GetVkImageView()
    {
        return vkImageView;
    }

    void ImageView::CreateImageView()
    {
        VkImageViewCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = image->GetVkImage();
        createInfo.viewType = VkConvert<ImageViewType, VkImageViewType>(config.type);
        createInfo.format = VkConvert<Format, VkFormat>(image->GetConfig().format);
        createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createInfo.subresourceRange.aspectMask = GetImageAspect(image);
        createInfo.subresourceRange.levelCount = config.mipLevelCount;
        createInfo.subresourceRange.baseMipLevel = config.baseMipLevel;
        createInfo.subresourceRange.layerCount = config.layerCount;
        createInfo.subresourceRange.baseArrayLayer = config.baseLayer;

        if (vkCreateImageView(device.GetVkDevice(), &createInfo, nullptr, &vkImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan image view");
        }
    }

    void ImageView::DestroyImageView()
    {
        vkDestroyImageView(device.GetVkDevice(), vkImageView, nullptr);
    }
}
