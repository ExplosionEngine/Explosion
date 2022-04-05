//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/Vulkan/SwapChain.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Common.h>
#include "PlatformSurface.h"

namespace RHI::Vulkan {

    VKSwapChain::VKSwapChain(VKInstance& instance, const SwapChainCreateInfo* createInfo) : SwapChain(createInfo)
    {
        CreateNativeSwapChain(instance, createInfo);
    }

    VKSwapChain::~VKSwapChain()
    {

    }

    Texture* VKSwapChain::GetTexture(uint8_t index)
    {
        return nullptr;
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

    void VKSwapChain::CreateNativeSwapChain(VKInstance& instance, const SwapChainCreateInfo* createInfo)
    {
        auto vkInstance = instance.GetInstance();
        surface = CreateNativeSurface(vkInstance, createInfo);

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
        // TODO

    }
}