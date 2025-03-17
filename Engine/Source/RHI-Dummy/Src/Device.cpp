//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Device.h>
#include <RHI/Dummy/Queue.h>
#include <RHI/Dummy/SwapChain.h>
#include <RHI/Dummy/Buffer.h>
#include <RHI/Dummy/Texture.h>
#include <RHI/Dummy/Sampler.h>
#include <RHI/Dummy/BindGroupLayout.h>
#include <RHI/Dummy/BindGroup.h>
#include <RHI/Dummy/PipelineLayout.h>
#include <RHI/Dummy/ShaderModule.h>
#include <RHI/Dummy/Pipeline.h>
#include <RHI/Dummy/CommandBuffer.h>
#include <RHI/Dummy/Synchronous.h>
#include <RHI/Dummy/Surface.h>
#include <Common/Debug.h>

namespace RHI::Dummy {
    DummyDevice::DummyDevice(DummyGpu& gpu, const DeviceCreateInfo& createInfo)
        : Device(createInfo)
        , gpu(gpu)
        , dummyQueue(Common::MakeUnique<DummyQueue>())
    {
    }

    DummyDevice::~DummyDevice() = default;

    DummyGpu& DummyDevice::GetGpu() const
    {
        return gpu;
    }

    size_t DummyDevice::GetQueueNum(const QueueType type)
    {
        return type == QueueType::graphics ? 1 : 0;
    }

    Queue* DummyDevice::GetQueue(const QueueType type, const size_t index)
    {
        Assert(type == QueueType::graphics && index == 0);
        return dummyQueue.Get();
    }

    Common::UniquePtr<Surface> DummyDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return { new DummySurface(createInfo) };
    }

    Common::UniquePtr<SwapChain> DummyDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        return { new DummySwapChain(createInfo) };
    }

    Common::UniquePtr<Buffer> DummyDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return { new DummyBuffer(createInfo) };
    }

    Common::UniquePtr<Texture> DummyDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return { new DummyTexture(createInfo) };
    }

    Common::UniquePtr<Sampler> DummyDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return { new DummySampler(createInfo) };
    }

    Common::UniquePtr<BindGroupLayout> DummyDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        return { new DummyBindGroupLayout(createInfo) };
    }

    Common::UniquePtr<BindGroup> DummyDevice::CreateBindGroup(const BindGroupCreateInfo& createInfo)
    {
        return { new DummyBindGroup(createInfo) };
    }

    Common::UniquePtr<PipelineLayout> DummyDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        return { new DummyPipelineLayout(createInfo) };
    }

    Common::UniquePtr<ShaderModule> DummyDevice::CreateShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        return { new DummyShaderModule(createInfo) };
    }

    Common::UniquePtr<ComputePipeline> DummyDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return { new DummyComputePipeline(createInfo) };
    }

    Common::UniquePtr<RasterPipeline> DummyDevice::CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo)
    {
        return { new DummyRasterPipeline(createInfo) };
    }

    Common::UniquePtr<CommandBuffer> DummyDevice::CreateCommandBuffer()
    {
        return { new DummyCommandBuffer() };
    }

    Common::UniquePtr<Fence> DummyDevice::CreateFence(const bool bInitAsSignaled)
    {
        return { new DummyFence(*this, bInitAsSignaled) };
    }

    Common::UniquePtr<Semaphore> DummyDevice::CreateSemaphore()
    {
        return { new DummySemaphore(*this) };
    }

    bool DummyDevice::CheckSwapChainFormatSupport(Surface* surface, PixelFormat format)
    {
        return true;
    }

    TextureSubResourceCopyFootprint DummyDevice::GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo)
    {
        return {};
    }
}
