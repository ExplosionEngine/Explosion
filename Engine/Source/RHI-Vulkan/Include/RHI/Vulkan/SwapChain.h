//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/SwapChain.h>
#include <vulkan/vulkan.h>
#include <vector>

namespace RHI::Vulkan {
    class VKDevice;
    class VKQueue;

    class VKSwapChain : public SwapChain {
    public:
        NonCopyable(VKSwapChain)
        explicit VKSwapChain(VKDevice& dev, const SwapChainCreateInfo& createInfo);
        ~VKSwapChain() override;

        Texture* GetTexture(uint8_t index) override;
        uint8_t AcquireBackTexture() override;
        void Present() override;
        void Destroy() override;

    private:
        void CreateNativeSwapChain(const SwapChainCreateInfo& createInfo);
        VKDevice& device;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<Texture*> textures;
        VkQueue queue = VK_NULL_HANDLE;
        uint32_t swapChainImageCount = 0;
        uint32_t currentImage = 0;
    };

}
