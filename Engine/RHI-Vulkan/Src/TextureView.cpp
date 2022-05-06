//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    static vk::ImageAspectFlags GetAspectMask(TextureAspect aspect)
    {
        static std::unordered_map<TextureAspect, vk::ImageAspectFlags> rules = {
            { TextureAspect::COLOR, vk::ImageAspectFlagBits::eColor },
            { TextureAspect::DEPTH, vk::ImageAspectFlagBits::eDepth},
            { TextureAspect::STENCIL, vk::ImageAspectFlagBits::eStencil },
            { TextureAspect::DEPTH_STENCIL, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil }
        };

        vk::ImageAspectFlags result = {};
        for (const auto& rule : rules) {
            if (aspect & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }

    VKTextureView::VKTextureView(VKTexture& tex, VKDevice& dev, const TextureViewCreateInfo* createInfo)
        : TextureView(createInfo), device(dev), vkTexture(tex), vkTextureView(VK_NULL_HANDLE)
    {
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

        if (device.GetVkDevice().createImageView(&viewInfo, nullptr, &vkTextureView) != vk::Result::eSuccess) {
            throw VKException("failed to create imageView");
        }
    }

}