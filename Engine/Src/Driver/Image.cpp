//
// Created by Administrator on 2021/4/23 0023.
//

#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/EnumAdapter.h>

namespace Explosion {
    Image::Image(Driver& driver, const Config& config)
        : driver(driver), device(*driver.GetDevice()), fromSwapChain(false), config(config)
    {
        CreateImage();
    }

    Image::Image(Driver& driver, const VkImage& vkImage, const Config& config)
        : driver(driver), device(*driver.GetDevice()), fromSwapChain(true), vkImage(vkImage), config(config) {}

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

    void Image::OnSetupImageCreateInfo(VkImageCreateInfo& createInfo) {}

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
        OnSetupImageCreateInfo(createInfo);

        if (vkCreateImage(device.GetVkDevice(), &createInfo, nullptr, &vkImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan image");
        }
    }

    void Image::DestroyImage()
    {
        vkDestroyImage(device.GetVkDevice(), vkImage, nullptr);
    }

    ColorAttachment::ColorAttachment(Driver& driver, const Config& config) : Image(driver, config), fromSwapChain(false) {}

    ColorAttachment::ColorAttachment(Driver& driver, const VkImage& vkImage, const Config& config) : Image(driver, vkImage, config), fromSwapChain(true) {}

    ColorAttachment::~ColorAttachment() = default;

    bool ColorAttachment::IsFromSwapChain()
    {
        return fromSwapChain;
    }

    void ColorAttachment::OnSetupImageCreateInfo(VkImageCreateInfo& createInfo)
    {
        Image::OnSetupImageCreateInfo(createInfo);
        createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
}
