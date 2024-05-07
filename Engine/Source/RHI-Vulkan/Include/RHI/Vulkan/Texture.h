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

        Common::UniqueRef<TextureView> CreateTextureView(const TextureViewCreateInfo& inCreateInfo) override;

        VkImage GetNative() const;
        VkImageSubresourceRange GetNativeSubResourceFullRange() const;

    private:
        void CreateNativeImage(const TextureCreateInfo& inCreateInfo);
        void GetAspect(const TextureCreateInfo& inCreateInfo);
        void TransitionToInitState(const TextureCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkImage nativeImage;
        VmaAllocation nativeAllocation;
        VkImageAspectFlags nativeAspect;
        bool ownMemory;
    };
}