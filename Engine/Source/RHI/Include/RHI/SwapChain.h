//
// Created by johnk on 28/3/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <RHI/Surface.h>

namespace RHI {
    class Texture;
    class Queue;
    class Fence;
    class Semaphore;

    struct SwapChainCreateInfo {
        Queue* presentQueue;
        Surface* surface;
        uint8_t textureNum;
        PixelFormat format;
        uint32_t width;
        uint32_t height;
        PresentMode presentMode;

        SwapChainCreateInfo();
        SwapChainCreateInfo& SetPresentQueue(Queue* inPresentQueue);
        SwapChainCreateInfo& SetSurface(Surface* inSurface);
        SwapChainCreateInfo& SetTextureNum(uint8_t inTextureNum);
        SwapChainCreateInfo& SetFormat(PixelFormat inFormat);
        SwapChainCreateInfo& SetWidth(uint32_t inWidth);
        SwapChainCreateInfo& SetHeight(uint32_t inHeight);
        SwapChainCreateInfo& SetPresentMode(PresentMode inMode);
    };

    class SwapChain {
    public:
        NonCopyable(SwapChain)
        virtual ~SwapChain();

        virtual uint8_t GetTextureNum() = 0;
        virtual Texture* GetTexture(uint8_t index) = 0;
        virtual uint8_t AcquireBackTexture(Semaphore* signalSemaphore) = 0;
        virtual void Present(Semaphore* waitSemaphore) = 0;

    protected:
        explicit SwapChain(const SwapChainCreateInfo& createInfo);
    };
}
