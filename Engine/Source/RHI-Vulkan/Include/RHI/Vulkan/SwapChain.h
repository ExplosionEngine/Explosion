//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/SwapChain.h>
#include <vulkan/vulkan.hpp>
#include <vector>

namespace RHI::Vulkan {
    class VKDevice;
    class VKQueue;

    vk::SurfaceKHR CreateNativeSurface(const vk::Instance& instance, const SwapChainCreateInfo& createInfo);

    class VKSwapChain : public SwapChain {
    public:
        NON_COPYABLE(VKSwapChain)
        explicit VKSwapChain(VKDevice& dev, const SwapChainCreateInfo& createInfo);
        ~VKSwapChain() override;

        Texture* GetTexture(uint8_t index) override;
        uint8_t AcquireBackTexture() override;
        void Present() override;
        void Destroy() override;

        vk::Semaphore GetImageSemaphore() const;
        void AddWaitSemaphore(vk::Semaphore);

    private:
        void CreateNativeSwapChain(const SwapChainCreateInfo& createInfo);
        VKDevice& device;
        vk::SwapchainKHR swapChain = VK_NULL_HANDLE;
        vk::SurfaceKHR surface = VK_NULL_HANDLE;
        std::vector<Texture*> textures;
        vk::Queue queue = VK_NULL_HANDLE;
        vk::Semaphore currentSemaphore;
        std::vector<vk::Semaphore> imageAvailableSemaphore;
        std::vector<vk::Semaphore> waitSemaphores;
        uint32_t swapChainImageCount = 0;
        uint32_t currentImage = 0;
    };

}
