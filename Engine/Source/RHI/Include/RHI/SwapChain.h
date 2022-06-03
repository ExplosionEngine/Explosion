//
// Created by johnk on 28/3/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class Texture;
    class Queue;

    struct SwapChainCreateInfo {
        Queue* presentQueue;
        void* window;
        uint8_t textureNum;
        PixelFormat format;
        Extent<2> extent;
        PresentMode presentMode;
    };

    class SwapChain {
    public:
        NON_COPYABLE(SwapChain)
        virtual ~SwapChain();

        virtual Texture* GetTexture(uint8_t index) = 0;
        virtual uint8_t GetBackTextureIndex() = 0;
        virtual void Present() = 0;
        virtual void Destroy() = 0;

    protected:
        explicit SwapChain(const SwapChainCreateInfo* createInfo);
    };
}
