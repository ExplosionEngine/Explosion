//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VulkanTextureView::VulkanTextureView(VulkanTexture& inTexture, VulkanDevice& nDevice, const TextureViewCreateInfo& inCreateInfo)
        : TextureView(inCreateInfo), device(nDevice), texture(inTexture), baseMipLevel(inCreateInfo.baseMipLevel), mipLevelNum(inCreateInfo.mipLevelNum), baseArrayLayer(inCreateInfo.baseArrayLayer), arrayLayerNum(inCreateInfo.arrayLayerNum)
    {
        CreateImageView(inCreateInfo);
    }

    VulkanTextureView::~VulkanTextureView()
    {
        DestroyImageView();
    }

    void VulkanTextureView::CreateImageView(const TextureViewCreateInfo& inCreateInfo)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.format = EnumCast<PixelFormat, VkFormat>(texture.GetFormat());
        viewInfo.image = texture.GetNative();
        viewInfo.viewType = EnumCast<TextureViewDimension, VkImageViewType>(inCreateInfo.dimension);
        viewInfo.subresourceRange = { EnumCast<TextureAspect, VkImageAspectFlags>(inCreateInfo.aspect), baseMipLevel, mipLevelNum, baseArrayLayer, arrayLayerNum };

        Assert(vkCreateImageView(device.GetNative(), &viewInfo, nullptr, &nativeImageView) == VK_SUCCESS);
    }

    void VulkanTextureView::DestroyImageView()
    {
        if (nativeImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(device.GetNative(), nativeImageView, nullptr);
        }
    }

    VkImageView VulkanTextureView::GetNative()
    {
        return nativeImageView;
    }

    VulkanTexture& VulkanTextureView::GetTexture() const
    {
        return texture;
    }

    uint8_t VulkanTextureView::GetArrayLayerNum() const
    {
        return arrayLayerNum;
    }
}