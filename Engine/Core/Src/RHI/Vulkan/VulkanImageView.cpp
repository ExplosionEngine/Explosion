//
// Created by John Kindem on 2021/4/24.
//

#include <Engine/RHI/Vulkan/VulkanImageView.h>
#include <Engine/RHI/Vulkan/VulkanDriver.h>
#include <Engine/RHI/Vulkan/VulkanImage.h>
#include <Engine/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
    VulkanImageView::VulkanImageView(VulkanDriver& driver, Config config)
        : ImageView(config), driver(driver), device(*driver.GetDevice())
    {
        CreateImageView();
    }

    VulkanImageView::~VulkanImageView()
    {
        DestroyImageView();
    }

    Image* VulkanImageView::GetImage()
    {
        return config.image;
    }

    const VkImageView& VulkanImageView::GetVkImageView()
    {
        return vkImageView;
    }

    void VulkanImageView::CreateImageView()
    {
        VkImageViewCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = dynamic_cast<VulkanImage*>(config.image)->GetVkImage();
        createInfo.viewType = VkConvert<ImageViewType, VkImageViewType>(config.type);
        createInfo.format = VkConvert<Format, VkFormat>(config.image->GetConfig().format);
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VkGetFlags<ImageAspectBits, VkImageAspectFlagBits>(config.aspects);
        createInfo.subresourceRange.levelCount = config.mipLevelCount;
        createInfo.subresourceRange.baseMipLevel = config.baseMipLevel;
        createInfo.subresourceRange.layerCount = config.layerCount;
        createInfo.subresourceRange.baseArrayLayer = config.baseLayer;

        if (vkCreateImageView(device.GetVkDevice(), &createInfo, nullptr, &vkImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan image view");
        }
    }

    void VulkanImageView::DestroyImageView()
    {
        vkDestroyImageView(device.GetVkDevice(), vkImageView, nullptr);
    }
}
