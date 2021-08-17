//
// Created by John Kindem on 2021/3/30.
//

#include <stdexcept>

#include <RHI/Vulkan/VulkanDriver.h>
#include <RHI/Vulkan/VulkanAdapater.h>
#include <RHI/Vulkan/VulkanImage.h>
#include <RHI/Vulkan/VulkanSwapChain.h>
#include <RHI/Vulkan/VulkanPlatform.h>
#include <RHI/Vulkan/VulkanUtils.h>
#include <RHI/Vulkan/VulkanSignal.h>

#ifdef _WIN32
#undef max
#undef min
#endif

namespace {
    const std::vector<Explosion::RHI::RateRule<VkSurfaceFormatKHR>> SURFACE_FORMAT_RATE_RULES = {
        [](const auto& surfaceFormat) -> uint32_t {
            switch (surfaceFormat.format) {
                case VK_FORMAT_B8G8R8A8_SRGB:
                    return 100;
                default:
                    return 0;
            }
        },
        [](const auto& surfaceFormat) -> uint32_t {
            switch (surfaceFormat.colorSpace) {
                case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
                    return 100;
                default:
                    return 0;
            }
        }
    };

    const std::vector<Explosion::RHI::RateRule<VkPresentModeKHR>> PRESENT_MODE_RATE_RULES = {
        [](const VkPresentModeKHR& presentMode) -> uint32_t {
            switch (presentMode) {
                case VK_PRESENT_MODE_MAILBOX_KHR:
                    return 100;
                case VK_PRESENT_MODE_FIFO_KHR:
                    return 50;
                default:
                    return 0;
            }
        }
    };
}

namespace Explosion::RHI {
    VulkanSwapChain::VulkanSwapChain(VulkanDriver& driver, Config config)
        : SwapChain(config), driver(driver), device(*driver.GetDevice())
    {
        CreateSurface();
        CheckPresentSupport();
        SelectSwapChainConfig();
        CreateSwapChain();
        FetchAttachments();
        CreateSignals();
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        DestroySignals();
        ClearAttachments();
        DestroySwapChain();
        DestroySurface();
    }

    void VulkanSwapChain::DoFrame(const FrameJob& frameJob)
    {
        uint32_t imageIdx;
        vkAcquireNextImageKHR(device.GetVkDevice(), vkSwapChain, UINT64_MAX, dynamic_cast<VulkanSignal*>(imageReadySignal)->GetVkSemaphore(), VK_NULL_HANDLE, &imageIdx);

        frameJob(imageIdx, imageReadySignal, frameFinishedSignal);

        VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &dynamic_cast<VulkanSignal*>(frameFinishedSignal)->GetVkSemaphore();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &vkSwapChain;
        presentInfo.pImageIndices = &imageIdx;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(device.GetVkQueue(), &presentInfo);
        vkQueueWaitIdle(device.GetVkQueue());
    }

    uint32_t VulkanSwapChain::GetColorAttachmentCount()
    {
        return colorAttachments.size();
    }

    Format VulkanSwapChain::GetSurfaceFormat()
    {
        return GetEnumByVk<VkFormat, Format>(vkSurfaceFormat.format);
    }

    const VkSurfaceKHR& VulkanSwapChain::GetVkSurface()
    {
        return vkSurface;
    }

    const VkSurfaceCapabilitiesKHR& VulkanSwapChain::GetVkSurfaceCapabilities()
    {
        return vkSurfaceCapabilities;
    }

    const VkExtent2D& VulkanSwapChain::GetVkExtent()
    {
        return vkExtent;
    }

    const VkSurfaceFormatKHR& VulkanSwapChain::GetVkSurfaceFormat()
    {
        return vkSurfaceFormat;
    }

    const VkPresentModeKHR& VulkanSwapChain::GetVkPresentMode()
    {
        return vkPresentMode;
    }

    Image* VulkanSwapChain::GetColorAttachment(uint32_t index)
    {
        return colorAttachments[index];
    }

    void VulkanSwapChain::CreateSurface()
    {
        if (!CreatePlatformSurface(device.GetVkInstance(), config.surface, vkSurface)) {
            throw std::runtime_error("failed to create vulkan surface");
        }
    }

    void VulkanSwapChain::DestroySurface()
    {
        vkDestroySurfaceKHR(device.GetVkInstance(), vkSurface, nullptr);
    }

    void VulkanSwapChain::CheckPresentSupport()
    {
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device.GetVkPhysicalDevice(), device.GetVkQueueFamilyIndex(), vkSurface, &presentSupport);
        if (!presentSupport) {
            throw std::runtime_error("selected queue family is not supporting presentation");
        }
    }

    void VulkanSwapChain::SelectSwapChainConfig()
    {
        // fetch surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetVkPhysicalDevice(), vkSurface, &vkSurfaceCapabilities);

        // choosing surface format
        uint32_t formatCnt;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetVkPhysicalDevice(), vkSurface, &formatCnt, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCnt);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetVkPhysicalDevice(), vkSurface, &formatCnt, surfaceFormats.data());
        vkSurfaceFormat = Rate<VkSurfaceFormatKHR>(surfaceFormats, SURFACE_FORMAT_RATE_RULES)[0].second;

        // choosing surface present mode
        uint32_t presentModeCnt;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetVkPhysicalDevice(), vkSurface, &presentModeCnt, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCnt);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetVkPhysicalDevice(), vkSurface, &presentModeCnt, presentModes.data());
        vkPresentMode = Rate<VkPresentModeKHR>(presentModes, PRESENT_MODE_RATE_RULES)[0].second;

        // choosing swap chain extent
        if (vkSurfaceCapabilities.currentExtent.width != UINT32_MAX) {
            vkExtent = vkSurfaceCapabilities.currentExtent;
        } else {
            VkExtent2D extent = { config.width, config.height };
            extent.width = std::max(vkSurfaceCapabilities.minImageExtent.width, std::min(vkSurfaceCapabilities.maxImageExtent.width, extent.width));
            extent.height = std::max(vkSurfaceCapabilities.minImageExtent.height, std::min(vkSurfaceCapabilities.maxImageExtent.height, extent.height));
            vkExtent = extent;
        }
    }

    void VulkanSwapChain::CreateSwapChain()
    {
        uint32_t imageCnt = vkSurfaceCapabilities.minImageCount + 1;
        if (vkSurfaceCapabilities.maxImageCount > 0 && imageCnt > vkSurfaceCapabilities.maxImageCount) {
            imageCnt = vkSurfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = vkSurface;
        createInfo.minImageCount = imageCnt;
        createInfo.imageFormat = vkSurfaceFormat.format;
        createInfo.imageColorSpace = vkSurfaceFormat.colorSpace;
        createInfo.imageExtent = vkExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.preTransform = vkSurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = vkPresentMode;
        createInfo.clipped = true;
        createInfo.oldSwapchain = nullptr;

        if (vkCreateSwapchainKHR(device.GetVkDevice(), &createInfo, nullptr, &vkSwapChain)) {
            throw std::runtime_error("failed to create swap chain");
        }
    }

    void VulkanSwapChain::DestroySwapChain()
    {
        vkDestroySwapchainKHR(device.GetVkDevice(), vkSwapChain, nullptr);
    }

    void VulkanSwapChain::FetchAttachments()
    {
        uint32_t imageCnt = 0;
        vkGetSwapchainImagesKHR(device.GetVkDevice(), vkSwapChain, &imageCnt, nullptr);
        std::vector<VkImage> images(imageCnt);
        vkGetSwapchainImagesKHR(device.GetVkDevice(), vkSwapChain, &imageCnt, images.data());

        colorAttachments.resize(imageCnt);
        VulkanImage::Config config {};
        config.imageType = ImageType::IMAGE_2D;
        config.format = GetEnumByVk<VkFormat, Format>(vkSurfaceFormat.format);
        config.width = vkExtent.width;
        config.height = vkExtent.height;
        config.depth = 1;
        config.mipLevels = 1;
        config.layers = 1;
        config.usages = FlagsCast(ImageUsageBits::COLOR_ATTACHMENT);
        config.initialLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        for (auto i  = 0; i < imageCnt; i++) {
            colorAttachments[i] = new VulkanImage(driver, images[i], config);
        }
    }

    void VulkanSwapChain::ClearAttachments()
    {
        for (auto* colorAttachment : colorAttachments) {
            delete colorAttachment;
        }
    }

    void VulkanSwapChain::CreateSignals()
    {
        imageReadySignal = driver.CreateSignal();
        frameFinishedSignal = driver.CreateSignal();
    }

    void VulkanSwapChain::DestroySignals()
    {
        driver.DestroySignal(imageReadySignal);
        driver.DestroySignal(frameFinishedSignal);
    }
}
