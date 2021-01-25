//
// Created by John Kindem on 2021/1/25 0025.
//

#include <driver/vulkan/vulkan-swap-chain.h>

namespace Explosion {
    VulkanSwapChain::VulkanSwapChain(VulkanContext& tContext, void* tSurface, uint32_t tWidth, uint32_t tHeight)
        : SwapChain(), context(tContext), surface(tSurface), width(tWidth), height(tHeight) {
        // TODO
    }

    VulkanSwapChain::~VulkanSwapChain() {
        // TODO
    }
}
