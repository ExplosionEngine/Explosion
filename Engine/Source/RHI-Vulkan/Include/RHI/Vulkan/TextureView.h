//
// Created by Zach Lee on 2022/3/7.
//


#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include <RHI/TextureView.h>

namespace RHI::Vulkan {
    class VulkanTexture;
    class VulkanDevice;

    class VulkanTextureView : public TextureView {
    public:
        NonCopyable(VulkanTextureView)
        VulkanTextureView(VulkanTexture& inTexture, VulkanDevice& nDevice, const TextureViewCreateInfo& inCreateInfo);
        ~VulkanTextureView() override;

        VkImageView GetNative() const;
        VulkanTexture& GetTexture() const;
        uint8_t GetArrayLayerNum() const;

    private:
        void CreateImageView(const TextureViewCreateInfo& inCreateInfo);
        void DestroyImageView() const;

        VulkanDevice& device;
        VulkanTexture& texture;
        uint8_t baseMipLevel;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
        VkImageView nativeImageView;
    };
}