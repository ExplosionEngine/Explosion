//
// Created by Zach Lee on 2022/3/7.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <RHI/TextureView.h>

namespace RHI::Vulkan {
    class VKTexture;
    class VKDevice;

    class VKTextureView : public TextureView {
    public:
        NonCopyable(VKTextureView)
        VKTextureView(VKTexture& tex, VKDevice& device, const TextureViewCreateInfo& createInfo);
        ~VKTextureView() override;
        void Destroy() override;

        VkImageView GetVkImageView();
        VKTexture& GetTexture() const;
        uint8_t GetArrayLayerNum() const;

    private:
        void CreateImageView(const TextureViewCreateInfo& createInfo);
        VKDevice& device;
        VKTexture& vkTexture;
        uint8_t baseMipLevel;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
    };
}