//
// Created by Zach Lee on 2022/4/4.
//

#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <RHI/SwapChain.h>

namespace RHI::Vulkan {
    class VulkanDevice;
    class VulkanQueue;

    class VulkanSwapChain final : public SwapChain {
    public:
        NonCopyable(VulkanSwapChain)
        explicit VulkanSwapChain(VulkanDevice& inDevice, const SwapChainCreateInfo& inCreateInfo);
        ~VulkanSwapChain() override;

        uint8_t GetTextureNum() override;
        Texture* GetTexture(uint8_t inIndex) override;
        uint8_t AcquireBackTexture(Semaphore* inSignalSemaphore) override;
        void Present(Semaphore* inWaitSemaphore) override;

    private:
        void CreateNativeSwapChain(const SwapChainCreateInfo& inCreateInfo);

        VulkanDevice& device;
        std::vector<Texture*> textures;
        VkSwapchainKHR nativeSwapChain;
        VkQueue nativeQueue;
        uint32_t swapChainImageCount = 0;
        uint32_t currentImage = 0;
    };
}
