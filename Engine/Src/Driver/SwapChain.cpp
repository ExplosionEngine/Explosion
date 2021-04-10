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
    }

    SwapChain::~SwapChain()
    {
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
                return;
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
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                vkPresentMode = presentMode;
                return;
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
        // TODO
    }

    void SwapChain::DestroySwapChain()
    {
        // TODO
    }
}
