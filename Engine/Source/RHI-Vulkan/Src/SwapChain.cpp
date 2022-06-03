//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/Vulkan/SwapChain.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Texture.h>
#include "PlatformSurface.h"

namespace RHI::Vulkan {

    VKSwapChain::VKSwapChain(VKDevice& dev, const SwapChainCreateInfo* createInfo)
        : device(dev), SwapChain(createInfo)
    {
        CreateNativeSwapChain(createInfo);
    }

    VKSwapChain::~VKSwapChain()
    {
        for (auto& tex : textures) {
            delete tex;
        }
        textures.clear();
    }

    Texture* VKSwapChain::GetTexture(uint8_t index)
    {
        return textures[index];
    }

    uint8_t VKSwapChain::GetBackTextureIndex()
    {
        return 0;
    }

    void VKSwapChain::Present()
    {

    }

    void VKSwapChain::Destroy()
    {
        delete this;
    }

    void VKSwapChain::CreateNativeSwapChain(const SwapChainCreateInfo* createInfo)
    {
        surface = CreateNativeSurface(device.GetGpu()->GetVKInstance(), createInfo);

        vk::SwapchainCreateInfoKHR swapChainInfo = {};
        swapChainInfo.setSurface(surface)
            .setImageFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo->format))
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
            .setImageExtent(vk::Extent2D{static_cast<uint32_t>(createInfo->extent.x), static_cast<uint32_t>(createInfo->extent.y)})
            .setClipped(VK_TRUE)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment);
        Assert(device.GetVkDevice().createSwapchainKHR(&swapChainInfo, nullptr, &swapChain) == vk::Result::eSuccess);

        TextureCreateInfo textureInfo = {};
        textureInfo.format = createInfo->format;
        textureInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::RENDER_ATTACHMENT;
        textureInfo.mipLevels = 1;
        textureInfo.samples = 1;
        textureInfo.dimension = TextureDimension::T_2D;
        textureInfo.extent.x = createInfo->extent.x;
        textureInfo.extent.y = createInfo->extent.y;
        textureInfo.extent.z = 1;

        auto images = device.GetVkDevice().getSwapchainImagesKHR(swapChain);
        for (auto& image : images) {
            textures.emplace_back(new VKTexture(device, &textureInfo, static_cast<vk::Image>(image)));
        }
    }
}