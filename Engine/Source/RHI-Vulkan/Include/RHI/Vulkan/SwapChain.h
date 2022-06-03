//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/SwapChain.h>
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {
    class VKInstance;

    class VKSwapChain : public SwapChain {
    public:
        NON_COPYABLE(VKSwapChain)
        explicit VKSwapChain(const vk::Instance& instance, const SwapChainCreateInfo* createInfo);
        ~VKSwapChain() override;

        Texture* GetTexture(uint8_t index) override;
        uint8_t GetBackTextureIndex() override;
        void Present() override;
        void Destroy() override;

    private:
        void CreateNativeSwapChain(const vk::Instance& instance, const SwapChainCreateInfo* createInfo);

        vk::SwapchainKHR swapChain = VK_NULL_HANDLE;
        vk::SurfaceKHR surface = VK_NULL_HANDLE;
    };

}
