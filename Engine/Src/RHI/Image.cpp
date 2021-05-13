//
// Created by Administrator on 2021/4/23 0023.
//

#include <utility>

#include <Explosion/RHI/Driver.h>
#include <Explosion/RHI/Image.h>
#include <Explosion/RHI/VkAdapater.h>

namespace Explosion::RHI {
    Image::Image(Driver& driver, Config config)
        : driver(driver), device(*driver.GetDevice()), fromSwapChain(false), config(std::move(config))
    {
        CreateImage();
    }

    Image::Image(Driver& driver, const VkImage& vkImage, Config config)
        : driver(driver), device(*driver.GetDevice()), fromSwapChain(true), vkImage(vkImage), config(std::move(config)) {}

    Image::~Image()
    {
        if (fromSwapChain) {
            return;
        }
        DestroyImage();
    }

    const VkImage& Image::GetVkImage()
    {
        return vkImage;
    }

    const Image::Config& Image::GetConfig()
    {
        return config;
    }

    void Image::CreateImage()
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
        createInfo.usage = VkGetFlags<ImageUsage, VkImageUsageFlagBits>(config.usages);
        createInfo.initialLayout = VkConvert<ImageLayout, VkImageLayout>(config.initialLayout);

        if (vkCreateImage(device.GetVkDevice(), &createInfo, nullptr, &vkImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan image");
        }
    }

    void Image::DestroyImage()
    {
        vkDestroyImage(device.GetVkDevice(), vkImage, nullptr);
    }
}
