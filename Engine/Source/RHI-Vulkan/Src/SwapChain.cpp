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

namespace RHI::Vulkan {
    VKSwapChain::VKSwapChain(VKDevice& dev, const SwapChainCreateInfo* createInfo)
        : device(dev), SwapChain(createInfo)
    {
        CreateNativeSwapChain(createInfo);
    }

    VKSwapChain::~VKSwapChain()
    {
        auto vkDevice = device.GetVkDevice();
        vkDevice.waitIdle();

        for (auto& tex : textures) {
            delete tex;
        }
        textures.clear();

        for (auto &semaphore : imageAvailableSemaphore) {
            vkDevice.destroySemaphore(semaphore);
        }

        if (swapChain) {
            vkDevice.destroySwapchainKHR(swapChain);
        }

        if (surface) {
            device.GetGpu().GetInstance().GetVkInstance().destroySurfaceKHR(surface);
        }

    }

    Texture* VKSwapChain::GetTexture(uint8_t index)
    {
        return textures[index];
    }

    uint8_t VKSwapChain::AcquireBackTexture()
    {
        currentSemaphore = imageAvailableSemaphore[currentImage];
        auto result = device.GetVkDevice().acquireNextImageKHR(swapChain, UINT64_MAX, currentSemaphore, {});
        currentImage = result.value;
        return currentImage;
    }

    void VKSwapChain::Present()
    {
        vk::PresentInfoKHR presetInfo{};
        presetInfo.setSwapchainCount(1)
            .setSwapchains(swapChain)
            .setWaitSemaphores(waitSemaphores)
            .setPImageIndices(&currentImage);
        Assert(queue.presentKHR(&presetInfo) == vk::Result::eSuccess);
        waitSemaphores.clear();
    }

    void VKSwapChain::Destroy()
    {
        delete this;
    }

    vk::Semaphore VKSwapChain::GetImageSemaphore() const
    {
        return currentSemaphore;
    }

    void VKSwapChain::AddWaitSemaphore(vk::Semaphore semaphore)
    {
        waitSemaphores.emplace_back(semaphore);
    }

    void VKSwapChain::CreateNativeSwapChain(const SwapChainCreateInfo* createInfo)
    {
        auto vkDevice = device.GetVkDevice();
        surface = CreateNativeSurface(device.GetGpu().GetInstance().GetVkInstance(), createInfo);

        auto* mQueue = dynamic_cast<VKQueue*>(createInfo->presentQueue);
        queue = mQueue->GetVkQueue();

        auto surfaceCap = device.GetGpu().GetVkPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
        vk::Extent2D extent = {static_cast<uint32_t>(createInfo->extent.x), static_cast<uint32_t>(createInfo->extent.y)};
        extent.width = std::clamp(extent.width, surfaceCap.minImageExtent.width, surfaceCap.maxImageExtent.width);
        extent.height = std::clamp(extent.height, surfaceCap.minImageExtent.height, surfaceCap.maxImageExtent.height);

        vk::Format supportedFormat = VKEnumCast<PixelFormat, vk::Format>(createInfo->format);
        vk::ColorSpaceKHR colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        auto surfaceFormats = device.GetGpu().GetVkPhysicalDevice().getSurfaceFormatsKHR(surface);
        {
            Assert(!surfaceFormats.empty());
            auto iter = std::find_if(surfaceFormats.begin(), surfaceFormats.end(),
                                     [supportedFormat, colorSpace, createInfo](vk::SurfaceFormatKHR format) {
                return supportedFormat == format.format && colorSpace == format.colorSpace;
            });
            if (iter == surfaceFormats.end()) {
                supportedFormat = surfaceFormats.begin()->format;
                colorSpace = surfaceFormats.begin()->colorSpace;
            }
        }

        auto presentModes = device.GetGpu().GetVkPhysicalDevice().getSurfacePresentModesKHR(surface);
        vk::PresentModeKHR supportedMode = vk::PresentModeKHR::eMailbox;
        {
            Assert(!presentModes.empty());
            auto iter = std::find_if(presentModes.begin(), presentModes.end(),
                                     [supportedMode](vk::PresentModeKHR mode) {
                                         return mode == supportedMode;
                                     });
            if (iter == presentModes.end()) {
                supportedMode = presentModes[0];
            }
        }

        swapChainImageCount = std::clamp(static_cast<uint32_t>(createInfo->textureNum), surfaceCap.minImageCount, surfaceCap.maxImageCount);
        vk::SwapchainCreateInfoKHR swapChainInfo = {};
        swapChainInfo.setSurface(surface)
            .setMinImageCount(swapChainImageCount)
            .setImageFormat(supportedFormat)
            .setImageColorSpace(colorSpace)
            .setPresentMode(supportedMode)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
            .setImageExtent(extent)
            .setClipped(VK_TRUE)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment);
        Assert(vkDevice.createSwapchainKHR(&swapChainInfo, nullptr, &swapChain) == vk::Result::eSuccess);

        TextureCreateInfo textureInfo = {};
        textureInfo.format = createInfo->format;
        textureInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::RENDER_ATTACHMENT;
        textureInfo.mipLevels = 1;
        textureInfo.samples = 1;
        textureInfo.dimension = TextureDimension::T_2D;
        textureInfo.extent.x = extent.width;
        textureInfo.extent.y = extent.height;
        textureInfo.extent.z = 1;

        auto images = vkDevice.getSwapchainImagesKHR(swapChain);
        for (auto& image : images) {
            textures.emplace_back(new VKTexture(device, &textureInfo, static_cast<vk::Image>(image)));
        }
        swapChainImageCount = static_cast<uint32_t>(images.size());

        imageAvailableSemaphore.resize(swapChainImageCount);
        for (uint32_t i = 0; i < swapChainImageCount; ++i) {
            imageAvailableSemaphore[i] = vkDevice.createSemaphore({}, nullptr);
        }
    }
}