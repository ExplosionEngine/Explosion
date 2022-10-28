//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/SwapChain.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLSwapChain::MTLSwapChain(MTLDevice& dev, const SwapChainCreateInfo* createInfo)
        : SwapChain(createInfo), mtlDevice(dev)
    {
        CreateNativeSwapChain(createInfo);
    }

    MTLSwapChain::~MTLSwapChain()
    {
        [view release];
        view = nullptr;
    }

    Texture* MTLSwapChain::GetTexture(uint8_t index)
    {
        return nullptr;
    }

    uint8_t MTLSwapChain::AcquireBackTexture()
    {
        drawables[currentImage] = [view.metalLayer nextDrawable];
        [drawables[currentImage] retain];
        return currentImage;
    }

    void MTLSwapChain::Present()
    {

    }

    void MTLSwapChain::Destroy()
    {
        delete this;
    }

    void MTLSwapChain::CreateNativeSwapChain(const SwapChainCreateInfo* createInfo)
    {
        nativeWindow = static_cast<NSWindow*>(createInfo->window);
        view = [[MetalView alloc] initWithFrame:nativeWindow
                                         device:mtlDevice.GetDevice()];
        [view retain];

        CGSize size = CGSizeMake(createInfo->extent.x, createInfo->extent.y);
        view.metalLayer.drawableSize = size;

        swapChainImageCount = createInfo->textureNum;
        drawables.resize(createInfo->textureNum);
    }
}
