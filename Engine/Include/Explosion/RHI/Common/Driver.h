//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <Explosion/RHI/Common/Buffer.h>
#include <Explosion/RHI/Common/Image.h>
#include <Explosion/RHI/Common/ImageView.h>
#include <Explosion/RHI/Common/FrameBuffer.h>
#include <Explosion/RHI/Common/SwapChain.h>
#include <Explosion/RHI/Common/RenderPass.h>
#include <Explosion/RHI/Common/GraphicsPipeline.h>
#include <Explosion/RHI/Common/CommandBuffer.h>
#include <Explosion/RHI/Common/Signal.h>
#include <Explosion/RHI/Common/DescriptorPool.h>

namespace Explosion::RHI {
    class Driver {
    public:
        virtual ~Driver();

        virtual Buffer* CreateBuffer(const Buffer::Config& config) = 0;
        virtual void DestroyBuffer(Buffer* buffer) = 0;

        virtual Image* CreateImage(const Image::Config& config) = 0;
        virtual void DestroyImage(Image* image) = 0;

        virtual ImageView* CreateImageView(const ImageView::Config& config) = 0;
        virtual void DestroyImageView(ImageView* imageView) = 0;

        virtual FrameBuffer* CreateFrameBuffer(const FrameBuffer::Config& config) = 0;
        virtual void DestroyFrameBuffer(FrameBuffer* frameBuffer) = 0;

        virtual SwapChain* CreateSwapChain(const SwapChain::Config& config) = 0;
        virtual void DestroySwapChain(SwapChain* swapChain) = 0;

        virtual RenderPass* CreateRenderPass(const RenderPass::Config& config) = 0;
        virtual void DestroyRenderPass(RenderPass* renderPass) = 0;

        virtual GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipeline::Config& config) = 0;
        virtual void DestroyGraphicsPipeline(GraphicsPipeline* graphicsPipeline) = 0;

        virtual CommandBuffer* CreateCommandBuffer() = 0;
        virtual void DestroyCommandBuffer(CommandBuffer* commandBuffer) = 0;

        virtual Signal* CreateSignal() = 0;
        virtual void DestroySignal(Signal* signal) = 0;

        virtual DescriptorPool* CreateDescriptorPool(const DescriptorPool::Config& config) = 0;
        virtual void DestroyDescriptorPool(DescriptorPool* descriptorPool) = 0;

    protected:
        Driver();
    };
}

#endif //EXPLOSION_DRIVER_H
