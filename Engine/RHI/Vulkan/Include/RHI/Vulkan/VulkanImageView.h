//
// Created by John Kindem on 2021/4/24.
//

#ifndef EXPLOSION_VULKANIMAGEVIEW_H
#define EXPLOSION_VULKANIMAGEVIEW_H

#include <vector>

#include <vulkan/vulkan.h>

#include <RHI/ImageView.h>
#include <RHI/Vulkan/Api.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanImage;

    class RHI_VULKAN_API VulkanImageView : public ImageView {
    public:
        VulkanImageView(VulkanDriver& driver, Config config);
        ~VulkanImageView() override;
        const VkImageView& GetVkImageView();
        Image* GetImage() override;

    private:
        void CreateImageView();
        void DestroyImageView();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANIMAGEVIEW_H
