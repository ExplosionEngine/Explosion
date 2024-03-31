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
    }

    void VulkanTextureView::Destroy()
    {
        delete this;
    }

    void VulkanTextureView::CreateImageView(const TextureViewCreateInfo& inCreateInfo)
    {
        if (!texture.nativeImageView) {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.format = VKEnumCast<PixelFormat, VkFormat>(texture.GetFormat());
            viewInfo.image = texture.GetNative();
            viewInfo.viewType = VKEnumCast<TextureViewDimension, VkImageViewType>(inCreateInfo.dimension);
            viewInfo.subresourceRange = {GetAspectMask(inCreateInfo.aspect), baseMipLevel, mipLevelNum, baseArrayLayer, arrayLayerNum };

            Assert(vkCreateImageView(device.GetNative(), &viewInfo, nullptr, &texture.nativeImageView) == VK_SUCCESS);
        }
    }

    VkImageView VulkanTextureView::GetNative()
    {
        return texture.nativeImageView;
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