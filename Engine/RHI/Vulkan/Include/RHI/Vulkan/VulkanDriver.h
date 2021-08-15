//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANDRIVER_H
#define EXPLOSION_VULKANDRIVER_H

#include <memory>

#include <RHI/Driver.h>
#include <RHI/Vulkan/VulkanDevice.h>

namespace Explosion::RHI {
    class VulkanDriver : public Driver {
    public:
        VulkanDriver();
        ~VulkanDriver() override;
        VulkanDevice* GetDevice();

        Buffer* CreateBuffer(const Buffer::Config& config) override;
        void DestroyBuffer(Buffer* buffer) override;

        Image* CreateImage(const Image::Config& config) override;
        void DestroyImage(Image* image) override;

        ImageView* CreateImageView(const ImageView::Config& config) override;
        void DestroyImageView(ImageView* imageView) override;

        FrameBuffer* CreateFrameBuffer(const FrameBuffer::Config& config) override;
        void DestroyFrameBuffer(FrameBuffer* frameBuffer) override;

        SwapChain* CreateSwapChain(const SwapChain::Config& config) override;
        void DestroySwapChain(SwapChain* swapChain) override;

        RenderPass* CreateRenderPass(const RenderPass::Config& config) override;
        void DestroyRenderPass(RenderPass* renderPass) override;

        GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipeline::Config& config) override;
        void DestroyGraphicsPipeline(GraphicsPipeline* graphicsPipeline) override;

        CommandBuffer* CreateCommandBuffer() override;
        void DestroyCommandBuffer(CommandBuffer* commandBuffer) override;

        Signal* CreateSignal() override;
        void DestroySignal(Signal* signal) override;

        DescriptorPool* CreateDescriptorPool(const DescriptorPool::Config& config) override;
        void DestroyDescriptorPool(DescriptorPool* descriptorPool) override;

        DescriptorSet* AllocateDescriptorSet(DescriptorPool* descriptorPool, GraphicsPipeline* pipeline) override;
        void FreeDescriptorSet(DescriptorSet* descriptorSet) override;

        Sampler* CreateSampler(const Sampler::Config& config) override;
        void DestroySampler(Sampler* sampler) override;

    private:
        std::unique_ptr<VulkanDevice> device;
    };
}

#endif //EXPLOSION_VULKANDRIVER_H
