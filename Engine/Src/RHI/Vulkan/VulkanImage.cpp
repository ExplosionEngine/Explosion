//
// Created by John Kindem on 2021/4/23 0023.
//

#include <utility>

#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanImage.h>
#include <Explosion/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
    VulkanImage::VulkanImage(VulkanDriver& driver, Config config)
        : driver(driver), device(*driver.GetDevice()), fromSwapChain(false), config(std::move(config))
    {
        CreateImage();
    }

    VulkanImage::VulkanImage(VulkanDriver& driver, const VkImage& vkImage, Config config)
        : driver(driver), device(*driver.GetDevice()), fromSwapChain(true), vkImage(vkImage), config(std::move(config)) {}

    VulkanImage::~VulkanImage()
    {
        if (fromSwapChain) {
            return;
        }
        DestroyImage();
    }

    const VkImage& VulkanImage::GetVkImage()
    {
        return vkImage;
    }

    const VulkanImage::Config& VulkanImage::GetConfig()
    {
        return config;
    }

    void VulkanImage::CreateImage()
    {
        VkImageCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.format = VkConvert<Format, VkFormat>(config.format);
        createInfo.extent = {
            static_cast<uint32_t>(config.width),
            static_cast<uint32_t>(config.height),
            static_cast<uint32_t>(config.depth)
        };
        createInfo.mipLevels = config.mipLevels;
        createInfo.arrayLayers = config.layers;
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage = 0;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        createInfo.usage = VkGetFlags<ImageUsageBits, VkImageUsageFlagBits>(config.usages);
        createInfo.initialLayout = VkConvert<ImageLayout, VkImageLayout>(config.initialLayout);

        if (vkCreateImage(device.GetVkDevice(), &createInfo, nullptr, &vkImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan image");
        }
    }

    void VulkanImage::DestroyImage()
    {
        vkDestroyImage(device.GetVkDevice(), vkImage, nullptr);
    }
}
