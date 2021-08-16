//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <Common/DynLib.h>
#include <RHI/Buffer.h>
#include <RHI/Image.h>
#include <RHI/ImageView.h>
#include <RHI/FrameBuffer.h>
#include <RHI/SwapChain.h>
#include <RHI/RenderPass.h>
#include <RHI/GraphicsPipeline.h>
#include <RHI/CommandBuffer.h>
#include <RHI/Signal.h>
#include <RHI/DescriptorPool.h>
#include <RHI/DescriptorSet.h>
#include <RHI/Sampler.h>

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

        virtual DescriptorSet* AllocateDescriptorSet(DescriptorPool* descriptorPool, GraphicsPipeline* pipeline) = 0;
        virtual void FreeDescriptorSet(DescriptorSet* descriptorSet) = 0;

        virtual Sampler* CreateSampler(const Sampler::Config& config) = 0;
        virtual void DestroySampler(Sampler* sampler) = 0;

    protected:
        Driver();
    };

    class DriverFactory {
    public:
        static DriverFactory& Singleton();

        ~DriverFactory() = default;
        DriverFactory(DriverFactory&) = delete;
        void operator=(DriverFactory&) = delete;

        Driver* CreateFromLib(const std::string& name);

    private:
        DriverFactory() = default;

        DynLib* FetchOrLoadLib(const std::string& name);

        std::unordered_map<std::string, std::unique_ptr<DynLib>> libs;
    };
}

#endif //EXPLOSION_DRIVER_H
