//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/SwapChain.h>
#include <Explosion/Driver/Platform.h>

namespace Explosion {
    SwapChain::SwapChain(Device& device, void* surface, uint32_t width, uint32_t height)
        : device(device), surface(surface), width(width), height(height)
    {
        CreateSurface();
        CheckPresentSupport();
        SelectSwapChainConfig();
        CreateSwapChain();
        GetImages();
        CreateImageViews();
    }

    SwapChain::~SwapChain()
    {
        DestroyImageViews();
        DestroySwapChain();
        DestroySurface();
    }

    const VkSurfaceKHR& SwapChain::GetVkSurface()
    {
        return vkSurface;
    }

    const VkSurfaceCapabilitiesKHR& SwapChain::GetVkSurfaceCapabilities()
    {
        return vkSurfaceCapabilities;
    }

    const VkExtent2D& SwapChain::GetVkExtent()
    {
        return vkExtent;
    }

    const VkSurfaceFormatKHR& SwapChain::GetVkSurfaceFormat()
    {
        return vkSurfaceFormat.value();
    }

    const VkPresentModeKHR& SwapChain::GetVkPresentMode()
    {
        return vkPresentMode.value();
    }

    const std::vector<VkImage>& SwapChain::GetVkImages()
    {
        return vkImages;
    }

    const std::vector<VkImageView>& SwapChain::GetVkImageViews()
    {
        return vkImageViews;
    }

    void SwapChain::CreateSurface()
    {
        if (!CreatePlatformSurface(device.GetVkInstance(), surface, vkSurface)) {
            throw std::runtime_error("failed to create vulkan surface");
        }
    }

    void SwapChain::DestroySurface()
    {
        vkDestroySurfaceKHR(device.GetVkInstance(), vkSurface, nullptr);
    }

    void SwapChain::CheckPresentSupport()
    {
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device.GetVkPhysicalDevice(), device.GetVkQueueFamilyIndex(), vkSurface, &presentSupport);
        if (!presentSupport) {
            throw std::runtime_error("selected queue family is not supporting presentation");
        }
    }

    // TODO: using rating to select surface format & present mode
    void SwapChain::SelectSwapChainConfig()
    {
        // fetch surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetVkPhysicalDevice(), vkSurface, &vkSurfaceCapabilities);

        // choosing surface format
        uint32_t formatCnt;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetVkPhysicalDevice(), vkSurface, &formatCnt, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCnt);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetVkPhysicalDevice(), vkSurface, &formatCnt, surfaceFormats.data());
        for (const auto& surfaceFormat : surfaceFormats) {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                vkSurfaceFormat = surfaceFormat;
                break;
            }
        }
        if (!vkSurfaceFormat.has_value()) {
            throw std::runtime_error("there is no suitable surface format");
        }

        // choosing surface present mode
        uint32_t presentModeCnt;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetVkPhysicalDevice(), vkSurface, &presentModeCnt, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCnt);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetVkPhysicalDevice(), vkSurface, &presentModeCnt, presentModes.data());
        for (const auto& presentMode : presentModes) {
            if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
                vkPresentMode = presentMode;
                break;
            }
        }
        if (!vkPresentMode.has_value()) {
            throw std::runtime_error("there is no suitable surface present mode");
        }

        // choosing swap chain extent
        if (vkSurfaceCapabilities.currentExtent.width != UINT32_MAX) {
            vkExtent = vkSurfaceCapabilities.currentExtent;
        } else {
            VkExtent2D extent = { width, height };
            extent.width = std::max(vkSurfaceCapabilities.minImageExtent.width, std::min(vkSurfaceCapabilities.maxImageExtent.width, extent.width));
            extent.height = std::max(vkSurfaceCapabilities.minImageExtent.height, std::min(vkSurfaceCapabilities.maxImageExtent.height, extent.height));
            vkExtent = extent;
        }
    }

    void SwapChain::CreateSwapChain()
    {
        uint32_t imageCnt = vkSurfaceCapabilities.minImageCount + 1;
        if (vkSurfaceCapabilities.maxImageCount > 0 && imageCnt > vkSurfaceCapabilities.maxImageCount) {
            imageCnt = vkSurfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = vkSurface;
        createInfo.minImageCount = imageCnt;
        createInfo.imageFormat = vkSurfaceFormat->format;
        createInfo.imageColorSpace = vkSurfaceFormat->colorSpace;
        createInfo.imageExtent = vkExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.preTransform = vkSurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = vkPresentMode.value();
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;

        if (vkCreateSwapchainKHR(device.GetVkDevice(), &createInfo, nullptr, &vkSwapChain)) {
            throw std::runtime_error("failed to create swap chain");
        }
    }

    void SwapChain::DestroySwapChain()
    {
        vkDestroySwapchainKHR(device.GetVkDevice(), vkSwapChain, nullptr);
    }

    void SwapChain::GetImages()
    {
        uint32_t imageCnt = 0;
        vkGetSwapchainImagesKHR(device.GetVkDevice(), vkSwapChain, &imageCnt, nullptr);
        vkImages.resize(imageCnt);
        vkGetSwapchainImagesKHR(device.GetVkDevice(), vkSwapChain, &imageCnt, vkImages.data());
    }

    void SwapChain::CreateImageViews()
    {
        vkImageViews.resize(vkImages.size());
        for (uint32_t i = 0; i < vkImageViews.size(); i++) {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = vkImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = vkSurfaceFormat->format;
            createInfo.components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            };
            createInfo.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT,
                0, 1,
                0, 1
            };

            if (vkCreateImageView(device.GetVkDevice(), &createInfo, nullptr, &vkImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image view");
            }
        }
    }

    void SwapChain::DestroyImageViews()
    {
        for (auto& imageView : vkImageViews) {
            vkDestroyImageView(device.GetVkDevice(), imageView, nullptr);
        }
        vkImageViews.clear();
    }
}
