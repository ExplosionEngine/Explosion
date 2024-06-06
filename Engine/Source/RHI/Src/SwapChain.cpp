//
// Created by johnk on 28/3/2022.
//

#include <RHI/SwapChain.h>

namespace RHI {
    SwapChainCreateInfo::SwapChainCreateInfo()
        : presentQueue(nullptr)
        , surface(nullptr)
        , textureNum(0)
        , format(PixelFormat::max)
        , width(0)
        , height(0)
        , presentMode(PresentMode::max)
    {
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetPresentQueue(Queue* inPresentQueue)
    {
        presentQueue = inPresentQueue;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetSurface(class Surface* inSurface)
    {
        surface = inSurface;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetTextureNum(uint8_t inTextureNum)
    {
        textureNum = inTextureNum;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetFormat(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetWidth(uint32_t inWidth)
    {
        width = inWidth;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetHeight(uint32_t inHeight)
    {
        height = inHeight;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::SetPresentMode(enum PresentMode inMode)
    {
        presentMode = inMode;
        return *this;
    }

    SwapChain::SwapChain(const SwapChainCreateInfo& createInfo) {}

    SwapChain::~SwapChain() = default;
}
