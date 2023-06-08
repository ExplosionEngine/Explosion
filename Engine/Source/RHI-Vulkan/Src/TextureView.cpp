//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VKTextureView::VKTextureView(VKTexture& tex, VKDevice& dev, const TextureViewCreateInfo& createInfo)
        : TextureView(createInfo), device(dev), vkTexture(tex), baseMipLevel(createInfo.baseMipLevel), mipLevelNum(createInfo.mipLevelNum), baseArrayLayer(createInfo.baseArrayLayer), arrayLayerNum(createInfo.arrayLayerNum)
    {
        CreateImageView(createInfo);
    }

    VKTextureView::~VKTextureView()
    {
    }

    void VKTextureView::Destroy()
    {
        delete this;
    }

    void VKTextureView::CreateImageView(const TextureViewCreateInfo& createInfo)
    {
        if (!vkTexture.vkImageView) {
            VkImageViewCreateInfo viewInfo = {};

            viewInfo.format = VKEnumCast<PixelFormat, VkFormat>(vkTexture.GetFormat());
            viewInfo.image = vkTexture.GetImage();
            viewInfo.viewType = VKEnumCast<TextureViewDimension, VkImageViewType>(createInfo.dimension);
            viewInfo.subresourceRange = { GetAspectMask(createInfo.aspect), baseMipLevel, mipLevelNum, baseArrayLayer, arrayLayerNum };

            Assert(vkCreateImageView(device.GetVkDevice(), &viewInfo, nullptr, &vkTexture.vkImageView) == VK_SUCCESS);
        }
    }

    VkImageView VKTextureView::GetVkImageView()
    {
        return vkTexture.vkImageView;
    }

    VKTexture& VKTextureView::GetTexture() const
    {
        return vkTexture;
    }

    uint8_t VKTextureView::GetArrayLayerNum() const
    {
        return arrayLayerNum;
    }
}