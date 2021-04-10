//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_SWAPCHAIN_H
#define EXPLOSION_SWAPCHAIN_H

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Device;

    class SwapChain {
    public:
        SwapChain(Device& device, void* surface, uint32_t width, uint32_t height);
        ~SwapChain();
        const VkSurfaceKHR& GetVkSurface();
        const VkSurfaceCapabilitiesKHR& GetVkSurfaceCapabilities();
        const VkExtent2D& GetVkExtent();
        const VkSurfaceFormatKHR& GetVkSurfaceFormat();
        const VkPresentModeKHR& GetVkPresentMode();
        const std::vector<VkImage>& GetVkImages();
        const std::vector<VkImageView>& GetVkImageViews();

    private:
        void CreateSurface();
        void DestroySurface();
        void CheckPresentSupport();

        void SelectSwapChainConfig();
        void CreateSwapChain();
        void DestroySwapChain();

        void GetImages();
        void CreateImageViews();
        void DestroyImageViews();

        Device& device;
        void* surface;
        uint32_t width;
        uint32_t height;
        VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
        VkSurfaceCapabilitiesKHR vkSurfaceCapabilities {};
        VkExtent2D vkExtent {};
        std::optional<VkSurfaceFormatKHR> vkSurfaceFormat;
        std::optional<VkPresentModeKHR> vkPresentMode;
        VkSwapchainKHR vkSwapChain = VK_NULL_HANDLE;
        std::vector<VkImage> vkImages {};
        std::vector<VkImageView> vkImageViews {};
    };
}

#endif //EXPLOSION_SWAPCHAIN_H
