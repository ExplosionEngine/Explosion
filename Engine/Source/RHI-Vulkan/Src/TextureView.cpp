//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VKTextureView::VKTextureView(VKTexture& tex, VKDevice& dev, const TextureViewCreateInfo* createInfo)
        : TextureView(createInfo), device(dev), vkTexture(tex), vkTextureView(VK_NULL_HANDLE)
    {
        Assert(createInfo != nullptr);
        baseMipLevel   = createInfo->baseMipLevel;
        mipLevelNum    = createInfo->mipLevelNum;
        baseArrayLayer = createInfo->baseArrayLayer;
        arrayLayerNum  = createInfo->arrayLayerNum;

        CreateImageView(createInfo);
    }

    VKTextureView::~VKTextureView()
    {
        if (vkTextureView) {
            device.GetVkDevice().destroyImageView(vkTextureView, nullptr);
        }
    }

    void VKTextureView::Destroy()
    {
        delete this;
    }

    void VKTextureView::CreateImageView(const TextureViewCreateInfo* createInfo)
    {
        vk::ImageViewCreateInfo viewInfo = {};

        viewInfo.setFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo->format))
            .setImage(vkTexture.GetImage())
            .setViewType(VKEnumCast<TextureViewDimension, vk::ImageViewType>(createInfo->dimension))
            .setSubresourceRange(vk::ImageSubresourceRange(GetAspectMask(createInfo->aspect),
                createInfo->baseMipLevel,
                createInfo->mipLevelNum,
                createInfo->baseArrayLayer,
                createInfo->arrayLayerNum
                ));

        Assert(device.GetVkDevice().createImageView(&viewInfo, nullptr, &vkTextureView) == vk::Result::eSuccess);
    }

    vk::ImageView VKTextureView::GetVkImageView()
    {
        return vkTextureView;
    }

    VKTexture& VKTextureView::GetTexture() const
    {
        return vkTexture;
    }

    uint8_t VKTextureView::GetBaseMipLevel() const
    {
        return baseMipLevel;
    }

    uint8_t VKTextureView::GetMipLevelNum() const
    {
        return mipLevelNum;
    }

    uint8_t VKTextureView::GetBaseArrayLayer() const
    {
        return baseArrayLayer;
    }

    uint8_t VKTextureView::GetArrayLayerNum() const
    {
        return arrayLayerNum;
    }
}