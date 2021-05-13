//
// Created by John Kindem on 2021/4/24.
//

#include <utility>

#include <Explosion/RHI/ImageView.h>
#include <Explosion/RHI/Driver.h>
#include <Explosion/RHI/Image.h>
#include <Explosion/RHI/VkAdapater.h>

namespace Explosion::RHI {
    ImageView::ImageView(Driver& driver, Config config)
        : driver(driver), device(*driver.GetDevice()), config(std::move(config))
    {
        CreateImageView();
    }

    ImageView::~ImageView()
    {
        DestroyImageView();
    }

    Image* ImageView::GetImage()
    {
        return config.image;
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
        createInfo.image = config.image->GetVkImage();
        createInfo.viewType = VkConvert<ImageViewType, VkImageViewType>(config.type);
        createInfo.format = VkConvert<Format, VkFormat>(config.image->GetConfig().format);
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VkGetFlags<ImageAspect, VkImageAspectFlagBits>(config.aspects);
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
