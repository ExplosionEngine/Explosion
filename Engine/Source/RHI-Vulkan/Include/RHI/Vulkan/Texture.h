//
// Created by Zach Lee on 2022/3/7.
//


#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <RHI/Texture.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanTexture : public Texture {
    public:
        NonCopyable(VulkanTexture)

        VulkanTexture(VulkanDevice& inDevice, const TextureCreateInfo& inCreateInfo, VkImage inNativeImage);
        VulkanTexture(VulkanDevice& inDevice, const TextureCreateInfo& inCreateInfo);
        ~VulkanTexture() override;

        void Destroy() override;

        TextureView* CreateTextureView(const TextureViewCreateInfo& inCreateInfo) override;

        VkImage GetNative() const;
        Common::UVec3 GetExtent() const;
        PixelFormat GetFormat() const;
        VkImageSubresourceRange GetNativeSubResourceFullRange();

    private:
        void CreateNativeImage(const TextureCreateInfo& inCreateInfo);
        void GetAspect(const TextureCreateInfo& inCreateInfo);
        void TransitionToInitState(const TextureCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkImage nativeImage;
        VmaAllocation nativeAllocation;
        VkImageAspectFlags nativeAspect = VK_IMAGE_ASPECT_COLOR_BIT;

        Common::UVec3 extent;
        PixelFormat format;
        uint8_t mipLevels;
        uint8_t samples;
        bool ownMemory;

        friend class VulkanTextureView;
        VkImageView nativeImageView = VK_NULL_HANDLE;
    };
}