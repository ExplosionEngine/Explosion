//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANSWAPCHAIN_H
#define EXPLOSION_VULKANSWAPCHAIN_H

#include <cstdint>
#include <optional>
#include <functional>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanSignal;
    class VulkanImage;
    class VulkanCommandBuffer;

    using FrameJob = std::function<void(uint32_t, VulkanSignal*, VulkanSignal*)>;

    class VulkanSwapChain {
    public:
        VulkanSwapChain(VulkanDriver& driver, void* surface, uint32_t width, uint32_t height);
        ~VulkanSwapChain();
        void DoFrame(const FrameJob& frameJob);
        uint32_t GetColorAttachmentCount();
        Format GetSurfaceFormat();
        const VkSurfaceKHR& GetVkSurface();
        const VkSurfaceCapabilitiesKHR& GetVkSurfaceCapabilities();
        const VkExtent2D& GetVkExtent();
        const VkSurfaceFormatKHR& GetVkSurfaceFormat();
        const VkPresentModeKHR& GetVkPresentMode();
        const std::vector<VulkanImage*>& GetColorAttachments();

    private:
        void CreateSurface();
        void DestroySurface();
        void CheckPresentSupport();

        void SelectSwapChainConfig();
        void CreateSwapChain();
        void DestroySwapChain();

        void FetchAttachments();

        void CreateSignals();
        void DestroySignals();

        VulkanDriver& driver;
        VulkanDevice& device;
        void* surface;
        uint32_t width;
        uint32_t height;
        VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
        VkSurfaceCapabilitiesKHR vkSurfaceCapabilities {};
        VkExtent2D vkExtent {};
        VkSurfaceFormatKHR vkSurfaceFormat;
        VkPresentModeKHR vkPresentMode;
        VkSwapchainKHR vkSwapChain = VK_NULL_HANDLE;
        std::vector<VulkanImage*> colorAttachments {};
        VulkanSignal* imageReadySignal;
        VulkanSignal* frameFinishedSignal;
    };
}

#endif //EXPLOSION_VULKANSWAPCHAIN_H
