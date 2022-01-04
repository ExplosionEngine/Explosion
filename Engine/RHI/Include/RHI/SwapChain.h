//
// Created by johnk on 2/1/2022.
//

#ifndef EXPLOSION_RHI_SWAP_CHAIN_H
#define EXPLOSION_RHI_SWAP_CHAIN_H

#include <RHI/Enum.h>
#include <Common/Utility.h>

namespace RHI {
    class Surface;
    class Queue;
    class Image;

    struct SwapChainCreateInfo {
        Queue* queue;
        Surface* surface;
        size_t imageNum;
        Extent2D extent;
        PixelFormat format;
    };

    class SwapChain {
    public:
        NON_COPYABLE(SwapChain)
        virtual ~SwapChain();

        virtual size_t GetImageNum() = 0;
        virtual Image* GetImage(size_t index) = 0;

    protected:
        explicit SwapChain(const SwapChainCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_SWAP_CHAIN_H
