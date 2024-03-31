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
        , extent(Common::UVec2Consts::zero)
        , presentMode(PresentMode::max)
    {
    }

    SwapChainCreateInfo& SwapChainCreateInfo::PresentQueue(Queue* inPresentQueue)
    {
        presentQueue = inPresentQueue;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::Surface(class Surface* inSurface)
    {
        surface = inSurface;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::TextureNum(uint8_t inTextureNum)
    {
        textureNum = inTextureNum;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::Format(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::Extent(const Common::UVec2& inExtent)
    {
        extent = inExtent;
        return *this;
    }

    SwapChainCreateInfo& SwapChainCreateInfo::PresentMode(enum PresentMode inMode)
    {
        presentMode = inMode;
        return *this;
    }

    SwapChain::SwapChain(const SwapChainCreateInfo& createInfo) {}

    SwapChain::~SwapChain() = default;
}
