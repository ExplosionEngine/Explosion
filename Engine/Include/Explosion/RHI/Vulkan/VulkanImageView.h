//
// Created by John Kindem on 2021/4/24.
//

#ifndef EXPLOSION_VULKANIMAGEVIEW_H
#define EXPLOSION_VULKANIMAGEVIEW_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanImage;

    class VulkanImageView {
    public:
        struct Config {
            VulkanImage* image;
            ImageViewType type;
            std::vector<ImageAspect> aspects;
            uint32_t mipLevelCount;
            uint32_t baseMipLevel;
            uint32_t layerCount;
            uint32_t baseLayer;
        };

        VulkanImageView(VulkanDriver& driver, Config config);
        ~VulkanImageView();
        VulkanImage* GetImage();
        const VkImageView& GetVkImageView();

    private:
        void CreateImageView();
        void DestroyImageView();

        VulkanDriver& driver;
        VulkanDevice& device;
        Config config {};
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANIMAGEVIEW_H
