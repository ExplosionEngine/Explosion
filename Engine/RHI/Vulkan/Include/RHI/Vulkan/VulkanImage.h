//
// Created by John Kindem on 2021/4/23 0023.
//

#ifndef EXPLOSION_VULKANIMAGE_H
#define EXPLOSION_VULKANIMAGE_H

#include <vulkan/vulkan.h>

#include <RHI/Image.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class VulkanImage : public Image {
    public:
        VulkanImage(VulkanDriver& driver, Config config);
        explicit VulkanImage(VulkanDriver& driver, const VkImage& vkImage, Config config);
        ~VulkanImage() override;

        const VkImage& GetVkImage();

    private:
        void CreateImage();
        void DestroyImage();

        VulkanDriver& driver;
        VulkanDevice& device;
        bool fromSwapChain = false;
        VkImage vkImage = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANIMAGE_H
