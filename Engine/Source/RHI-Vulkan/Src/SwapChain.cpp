//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/Vulkan/SwapChain.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/Surface.h>

namespace RHI::Vulkan {
    VKSwapChain::VKSwapChain(VKDevice& dev, const SwapChainCreateInfo& createInfo)
        : device(dev), SwapChain(createInfo)
    {
        CreateNativeSwapChain(createInfo);
    }

    VKSwapChain::~VKSwapChain()
    {
        auto vkDevice = device.GetVkDevice();
        vkDeviceWaitIdle(vkDevice);

        for (auto& tex : textures) {
            delete tex;
        }
        textures.clear();

        if (swapChain) {
            vkDestroySwapchainKHR(vkDevice, swapChain, nullptr);
        }
    }

    Texture* VKSwapChain::GetTexture(uint8_t index)
    {
        return textures[index];
    }

    uint8_t VKSwapChain::AcquireBackTexture(Fence* fence, uint32_t waitFenceValue)
    {
        // TODO wait fence

        Assert(vkAcquireNextImageKHR(device.GetVkDevice(), swapChain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &currentImage) == VK_SUCCESS);
        return currentImage;
    }

    void VKSwapChain::Present()
    {
        VkPresentInfoKHR presetInfo = {};
        presetInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presetInfo.swapchainCount = 1;
        presetInfo.pSwapchains = &swapChain;
        presetInfo.waitSemaphoreCount = 0;
        presetInfo.pWaitSemaphores = nullptr;
        presetInfo.pImageIndices = &currentImage;
        Assert(vkQueuePresentKHR(queue, &presetInfo) == VK_SUCCESS);
    }

    void VKSwapChain::Destroy()
    {
        delete this;
    }

    void VKSwapChain::CreateNativeSwapChain(const SwapChainCreateInfo& createInfo)
    {
        auto vkDevice = device.GetVkDevice();
        auto* mQueue = static_cast<VKQueue*>(createInfo.presentQueue);
        Assert(mQueue);
        auto* vkSurface = static_cast<VKSurface*>(createInfo.surface);
        Assert(vkSurface);
        queue = mQueue->GetVkQueue();
        auto surface = vkSurface->GetVKSurface();

        VkSurfaceCapabilitiesKHR surfaceCap;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetGpu().GetVkPhysicalDevice(), surface, &surfaceCap);

        VkExtent2D extent = {static_cast<uint32_t>(createInfo.extent.x), static_cast<uint32_t>(createInfo.extent.y)};
        extent.width = std::clamp(extent.width, surfaceCap.minImageExtent.width, surfaceCap.maxImageExtent.width);
        extent.height = std::clamp(extent.height, surfaceCap.minImageExtent.height, surfaceCap.maxImageExtent.height);

        Assert(device.CheckSwapChainFormatSupport(vkSurface, createInfo.format));
        auto supportedFormat = VKEnumCast<PixelFormat, VkFormat>(createInfo.format);
        auto colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        uint32_t presentModeCount = 0;
        std::vector<VkPresentModeKHR> presentModes;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetGpu().GetVkPhysicalDevice(), surface, &presentModeCount, nullptr);
        Assert(presentModeCount != 0);
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetGpu().GetVkPhysicalDevice(), surface, &presentModeCount, presentModes.data());

        VkPresentModeKHR supportedMode = VKEnumCast<PresentMode, VkPresentModeKHR>(createInfo.presentMode);
        {
            Assert(!presentModes.empty());
            auto iter = std::find_if(presentModes.begin(), presentModes.end(),
                                     [supportedMode](VkPresentModeKHR mode) {
                                         return mode == supportedMode;
                                     });
            Assert(iter != presentModes.end());
        }

        swapChainImageCount = std::clamp(static_cast<uint32_t>(createInfo.textureNum), surfaceCap.minImageCount, surfaceCap.maxImageCount);
        VkSwapchainCreateInfoKHR swapChainInfo = {};
        swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainInfo.surface = surface;
        swapChainInfo.minImageCount = swapChainImageCount;
        swapChainInfo.imageFormat = supportedFormat;
        swapChainInfo.imageColorSpace = colorSpace;
        swapChainInfo.presentMode = supportedMode;
        swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapChainInfo.imageExtent = extent;
        swapChainInfo.clipped = VK_TRUE;
        swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainInfo.imageArrayLayers = 1;
        swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        Assert(vkCreateSwapchainKHR(vkDevice, &swapChainInfo, nullptr, &swapChain) == VK_SUCCESS);

        TextureCreateInfo textureInfo = {};
        textureInfo.format = createInfo.format;
        textureInfo.usages = TextureUsageBits::copyDst | TextureUsageBits::renderAttachment;
        textureInfo.mipLevels = 1;
        textureInfo.samples = 1;
        textureInfo.dimension = TextureDimension::t2D;
        textureInfo.extent.x = extent.width;
        textureInfo.extent.y = extent.height;
        textureInfo.extent.z = 1;

        vkGetSwapchainImagesKHR(device.GetVkDevice(), swapChain, &swapChainImageCount, nullptr);
        std::vector<VkImage> swapChainImages(swapChainImageCount);
        vkGetSwapchainImagesKHR(device.GetVkDevice(), swapChain, &swapChainImageCount, swapChainImages.data());
        for (auto& image : swapChainImages) {
            textures.emplace_back(new VKTexture(device, textureInfo, image));
        }
        swapChainImageCount = static_cast<uint32_t>(swapChainImages.size());
    }
}