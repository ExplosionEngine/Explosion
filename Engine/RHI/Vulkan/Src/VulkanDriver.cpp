//
// Created by John Kindem on 2021/3/30.
//

#include <RHI/Vulkan/VulkanBuffer.h>
#include <RHI/Vulkan/VulkanDriver.h>
#include <RHI/Vulkan/VulkanImage.h>
#include <RHI/Vulkan/VulkanImageView.h>
#include <RHI/Vulkan/VulkanFrameBuffer.h>
#include <RHI/Vulkan/VulkanSwapChain.h>
#include <RHI/Vulkan/VulkanRenderPass.h>
#include <RHI/Vulkan/VulkanGraphicsPipeline.h>
#include <RHI/Vulkan/VulkanCommandBuffer.h>
#include <RHI/Vulkan/VulkanSignal.h>
#include <RHI/Vulkan/VulkanDescriptorPool.h>
#include <RHI/Vulkan/VulkanDescriptorSet.h>
#include <RHI/Vulkan/VulkanSampler.h>

namespace Explosion::RHI {
    VulkanDriver::VulkanDriver() : device(std::make_unique<VulkanDevice>(*this)) {}

    VulkanDriver::~VulkanDriver() = default;

    VulkanDevice* VulkanDriver::GetDevice()
    {
        return device.get();
    }

    Buffer* VulkanDriver::CreateBuffer(const Buffer::Config& config)
    {
        return static_cast<Buffer*>(new VulkanBuffer(*this, config));
    }

    void VulkanDriver::DestroyBuffer(Buffer* buffer)
    {
        delete buffer;
    }

    Image* VulkanDriver::CreateImage(const Image::Config& config)
    {
        return static_cast<Image*>(new VulkanImage(*this, config));
    }

    void VulkanDriver::DestroyImage(Image* image)
    {
        delete image;
    }

    ImageView* VulkanDriver::CreateImageView(const ImageView::Config& config)
    {
        return static_cast<ImageView*>(new VulkanImageView(*this, config));
    }

    void VulkanDriver::DestroyImageView(ImageView* imageView)
    {
        delete imageView;
    }

    FrameBuffer* VulkanDriver::CreateFrameBuffer(const FrameBuffer::Config& config)
    {
        return static_cast<FrameBuffer*>(new VulkanFrameBuffer(*this, config));
    }

    void VulkanDriver::DestroyFrameBuffer(FrameBuffer* frameBuffer)
    {
        delete frameBuffer;
    }

    SwapChain* VulkanDriver::CreateSwapChain(const SwapChain::Config& config)
    {
        return static_cast<SwapChain*>(new VulkanSwapChain(*this, config));
    }

    void VulkanDriver::DestroySwapChain(SwapChain* swapChain)
    {
        delete swapChain;
    }

    RenderPass* VulkanDriver::CreateRenderPass(const RenderPass::Config& config)
    {
        return static_cast<RenderPass*>(new VulkanRenderPass(*this, config));
    }

    void VulkanDriver::DestroyRenderPass(RenderPass* renderPass)
    {
        delete renderPass;
    }

    GraphicsPipeline* VulkanDriver::CreateGraphicsPipeline(const GraphicsPipeline::Config& config)
    {
        return static_cast<GraphicsPipeline*>(new VulkanGraphicsPipeline(*this, config));
    }

    void VulkanDriver::DestroyGraphicsPipeline(GraphicsPipeline* graphicsPipeline)
    {
        delete graphicsPipeline;
    }

    CommandBuffer* VulkanDriver::CreateCommandBuffer()
    {
        return static_cast<CommandBuffer*>(new VulkanCommandBuffer(*this));
    }

    void VulkanDriver::DestroyCommandBuffer(CommandBuffer* commandBuffer)
    {
        delete commandBuffer;
    }

    Signal* VulkanDriver::CreateSignal()
    {
        return static_cast<Signal*>(new VulkanSignal(*this));
    }

    void VulkanDriver::DestroySignal(Signal* signal)
    {
        delete signal;
    }

    DescriptorPool* VulkanDriver::CreateDescriptorPool(const DescriptorPool::Config& config)
    {
        return static_cast<DescriptorPool*>(new VulkanDescriptorPool(*this, config));
    }

    void VulkanDriver::DestroyDescriptorPool(DescriptorPool* descriptorPool)
    {
        delete descriptorPool;
    }

    DescriptorSet* VulkanDriver::AllocateDescriptorSet(DescriptorPool* descriptorPool, GraphicsPipeline* pipeline)
    {
        return static_cast<DescriptorSet*>(new VulkanDescriptorSet(*this, dynamic_cast<VulkanDescriptorPool*>(descriptorPool), dynamic_cast<VulkanGraphicsPipeline*>(pipeline)));
    }

    void VulkanDriver::FreeDescriptorSet(DescriptorSet* descriptorSet)
    {
        delete descriptorSet;
    }

    Sampler* VulkanDriver::CreateSampler(const Sampler::Config& config)
    {
        return static_cast<Sampler*>(new VulkanSampler(*this, config));
    }

    void VulkanDriver::DestroySampler(Sampler* sampler)
    {
        delete sampler;
    }
}

extern "C" {
    Explosion::RHI::Driver* CreateDriver()
    {
        return new Explosion::RHI::VulkanDriver;
    }
}