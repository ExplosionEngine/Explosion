//
// Created by johnk on 28/3/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <RHI/Surface.h>

namespace RHI {
    class Texture;
    class Queue;

    struct SwapChainCreateInfo {
        Queue* presentQueue;
        Surface* surface;
        uint8_t textureNum;
        PixelFormat format;
        Extent<2> extent;
        PresentMode presentMode;
    };

    class SwapChain {
    public:
        NonCopyable(SwapChain)
        virtual ~SwapChain();

        virtual Texture* GetTexture(uint8_t index) = 0;
        virtual uint8_t AcquireBackTexture() = 0;
        virtual void Present() = 0;
        virtual void Destroy() = 0;

    protected:
        explicit SwapChain(const SwapChainCreateInfo& createInfo);
    };
}
