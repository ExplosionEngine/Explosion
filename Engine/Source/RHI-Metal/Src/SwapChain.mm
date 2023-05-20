//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/SwapChain.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLSwapChain::MTLSwapChain(MTLDevice& dev, const SwapChainCreateInfo& createInfo)
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
        return textures[index].Get();
    }

    uint8_t MTLSwapChain::AcquireBackTexture()
    {
        drawables[currentImage] = [view.metalLayer nextDrawable];
        textures[currentImage]->SetDrawable(drawables[currentImage]);
        return currentImage;
    }

    void MTLSwapChain::Present()
    {
//        MTLQueue *queue = static_cast<MTLQueue*>(mtlDevice.GetQueue(QueueType::graphics, 0));
//        id<MTLCommandBuffer> commandBuffer = [queue->GetNativeQueue() commandBuffer];
//        [commandBuffer encodeWaitForEvent:event value:1];
//        [commandBuffer encodeSignalEvent:event value:0];
//        [commandBuffer presentDrawable:drawables[currentImage]];
//        [commandBuffer commit];

        textures[currentImage]->ResetDrawable();
        drawables[currentImage] = nil;
        currentImage = (currentImage + 1) % swapChainImageCount;
    }

    void MTLSwapChain::Destroy()
    {
        delete this;
    }

    void MTLSwapChain::AddSignalEventToCmd(id<MTLCommandBuffer> commandBuffer)
    {
        [commandBuffer presentDrawable:drawables[currentImage]];
//        [commandBuffer encodeSignalEvent:event value:1];
    }

    void MTLSwapChain::CreateNativeSwapChain(const SwapChainCreateInfo& createInfo)
    {
        nativeWindow = static_cast<NSWindow*>(createInfo.window);
        view = [[MetalView alloc] initWithFrame:nativeWindow
                                         device:mtlDevice.GetDevice()];
        CGSize size = CGSizeMake(createInfo.extent.x, createInfo.extent.y);
        [view setFrameSize: size];
//        view.metalLayer.drawableSize = size;
        
        [view retain];
        
        nativeWindow.contentView = view;

        swapChainImageCount = createInfo.textureNum;
        drawables.resize(createInfo.textureNum);
        textures.resize(createInfo.textureNum);
        for (uint32_t i = 0; i < createInfo.textureNum; ++i) {
            textures[i] = Common::MakeUnique<MTLTexture>(mtlDevice);
        }

        event = [mtlDevice.GetDevice() newEvent];
    }
}
