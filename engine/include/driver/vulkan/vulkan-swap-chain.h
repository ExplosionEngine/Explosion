//
// Created by John Kindem on 2021/1/25 0025.
//

#ifndef EXPLOSION_VULKAN_SWAP_CHAIN_H
#define EXPLOSION_VULKAN_SWAP_CHAIN_H

#include <driver/swap-chain.h>
#include <driver/vulkan/vulkan-context.h>

namespace Explosion {
    class VulkanSwapChain : public SwapChain {
    public:
        VulkanSwapChain(VulkanContext& tContext, void* tSurface, uint32_t tWidth, uint32_t tHeight);
        ~VulkanSwapChain() override;

    private:
        VulkanContext& context;
        void* surface = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
    };
}

#endif //EXPLOSION_VULKAN_SWAP_CHAIN_H
