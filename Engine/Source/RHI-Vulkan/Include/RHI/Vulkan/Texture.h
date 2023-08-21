//
// Created by Zach Lee on 2022/3/7.
//


#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <RHI/Texture.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKTexture : public Texture {
    public:
        NonCopyable(VKTexture)

        VKTexture(VKDevice& device, const TextureCreateInfo& createInfo, VkImage image);
        VKTexture(VKDevice& device, const TextureCreateInfo& createInfo);
        ~VKTexture() override;

        void Destroy() override;

        TextureView* CreateTextureView(const TextureViewCreateInfo& createInfo) override;

        VkImage GetImage() const;
        Extent<3> GetExtent() const;
        PixelFormat GetFormat() const;
        VkImageSubresourceRange GetFullRange();

    private:
        void CreateImage(const TextureCreateInfo& createInfo);
        void GetAspect(const TextureCreateInfo& createInfo);
        void TransitionToInitState(const TextureCreateInfo& createInfo);

        VKDevice& device;
        VkImage vkImage;
        VmaAllocation allocation;
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;

        Extent<3> extent;
        PixelFormat format;
        uint8_t mipLevels;
        uint8_t samples;
        bool ownMemory;

        friend class VKTextureView;
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}