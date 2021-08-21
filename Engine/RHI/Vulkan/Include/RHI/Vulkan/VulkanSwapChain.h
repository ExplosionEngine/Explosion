//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANSWAPCHAIN_H
#define EXPLOSION_VULKANSWAPCHAIN_H

#include <cstdint>
#include <optional>
#include <functional>

#include <vulkan/vulkan.h>

#include <RHI/SwapChain.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanSignal;
    class VulkanImage;
    class VulkanCommandBuffer;

    class VulkanSwapChain : public SwapChain {
    public:
        VulkanSwapChain(VulkanDriver& driver, Config config);
        ~VulkanSwapChain() override;
        void DoFrame(const FrameJob& frameJob) override;
        uint32_t GetColorAttachmentCount() override;
        Format GetSurfaceFormat() override;
        Image* GetColorAttachment(uint32_t index) override;

        const VkSurfaceKHR& GetVkSurface();
        const VkSurfaceCapabilitiesKHR& GetVkSurfaceCapabilities();
        const VkExtent2D& GetVkExtent();
        const VkSurfaceFormatKHR& GetVkSurfaceFormat();
        const VkPresentModeKHR& GetVkPresentMode();

    private:
        void CreateSurface();
        void DestroySurface();
        void CheckPresentSupport();

        void SelectSwapChainConfig();
        void CreateSwapChain();
        void DestroySwapChain();

        void FetchAttachments();
        void ClearAttachments();

        void CreateSignals();
        void DestroySignals();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
        VkSurfaceCapabilitiesKHR vkSurfaceCapabilities {};
        VkExtent2D vkExtent {};
        VkSurfaceFormatKHR vkSurfaceFormat;
        VkPresentModeKHR vkPresentMode;
        VkSwapchainKHR vkSwapChain = VK_NULL_HANDLE;
        std::vector<VulkanImage*> colorAttachments {};
        Signal* imageReadySignal;
        Signal* frameFinishedSignal;
    };
}

#endif //EXPLOSION_VULKANSWAPCHAIN_H
