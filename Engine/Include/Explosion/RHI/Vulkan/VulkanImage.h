//
// Created by John Kindem on 2021/4/23 0023.
//

#ifndef EXPLOSION_VULKANIMAGE_H
#define EXPLOSION_VULKANIMAGE_H

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class VulkanImage {
    public:
        struct Config {
            ImageType imageType;
            Format format;
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mipLevels;
            uint32_t layers;
            ImageLayout initialLayout;
            BufferUsageFlags usages;
        };

        VulkanImage(VulkanDriver& driver, Config config);
        explicit VulkanImage(VulkanDriver& driver, const VkImage& vkImage, Config config);
        ~VulkanImage();

        const VkImage& GetVkImage();
        const Config& GetConfig();

    private:
        void CreateImage();
        void DestroyImage();

        VulkanDriver& driver;
        VulkanDevice& device;
        Config config {};
        bool fromSwapChain = false;
        VkImage vkImage = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANIMAGE_H
