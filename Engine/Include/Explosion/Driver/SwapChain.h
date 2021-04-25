//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_SWAPCHAIN_H
#define EXPLOSION_SWAPCHAIN_H

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Driver;
    class ColorAttachment;

    class SwapChain {
    public:
        SwapChain(Driver& driver, void* surface, uint32_t width, uint32_t height);
        ~SwapChain();
        uint32_t GetColorAttachmentCount();
        const VkSurfaceKHR& GetVkSurface();
        const VkSurfaceCapabilitiesKHR& GetVkSurfaceCapabilities();
        const VkExtent2D& GetVkExtent();
        const VkSurfaceFormatKHR& GetVkSurfaceFormat();
        const VkPresentModeKHR& GetVkPresentMode();
        const std::vector<ColorAttachment*>& GetColorAttachments();

    private:
        void CreateSurface();
        void DestroySurface();
        void CheckPresentSupport();

        void SelectSwapChainConfig();
        void CreateSwapChain();
        void DestroySwapChain();

        void FetchAttachments();

        Driver& driver;
        Device& device;
        void* surface;
        uint32_t width;
        uint32_t height;
        VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
        VkSurfaceCapabilitiesKHR vkSurfaceCapabilities {};
        VkExtent2D vkExtent {};
        VkSurfaceFormatKHR vkSurfaceFormat;
        VkPresentModeKHR vkPresentMode;
        VkSwapchainKHR vkSwapChain = VK_NULL_HANDLE;
        std::vector<ColorAttachment*> colorAttachments {};
    };
}

#endif //EXPLOSION_SWAPCHAIN_H
