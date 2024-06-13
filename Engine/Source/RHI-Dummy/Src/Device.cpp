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
    DummyDevice::DummyDevice(const DeviceCreateInfo& createInfo)
        : Device(createInfo)
        , dummyQueue(Common::MakeUnique<DummyQueue>())
    {
    }

    DummyDevice::~DummyDevice() = default;

    size_t DummyDevice::GetQueueNum(const QueueType type)
    {
        return type == QueueType::graphics ? 1 : 0;
    }

    Queue* DummyDevice::GetQueue(const QueueType type, const size_t index)
    {
        Assert(type == QueueType::graphics && index == 0);
        return dummyQueue.Get();
    }

    Common::UniqueRef<Surface> DummyDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return { new DummySurface(createInfo) };
    }

    Common::UniqueRef<SwapChain> DummyDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        return { new DummySwapChain(createInfo) };
    }

    Common::UniqueRef<Buffer> DummyDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return { new DummyBuffer(createInfo) };
    }

    Common::UniqueRef<Texture> DummyDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return { new DummyTexture(createInfo) };
    }

    Common::UniqueRef<Sampler> DummyDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return { new DummySampler(createInfo) };
    }

    Common::UniqueRef<BindGroupLayout> DummyDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        return { new DummyBindGroupLayout(createInfo) };
    }

    Common::UniqueRef<BindGroup> DummyDevice::CreateBindGroup(const BindGroupCreateInfo& createInfo)
    {
        return { new DummyBindGroup(createInfo) };
    }

    Common::UniqueRef<PipelineLayout> DummyDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        return { new DummyPipelineLayout(createInfo) };
    }

    Common::UniqueRef<ShaderModule> DummyDevice::CreateShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        return { new DummyShaderModule(createInfo) };
    }

    Common::UniqueRef<ComputePipeline> DummyDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return { new DummyComputePipeline(createInfo) };
    }

    Common::UniqueRef<RasterPipeline> DummyDevice::CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo)
    {
        return { new DummyRasterPipeline(createInfo) };
    }

    Common::UniqueRef<CommandBuffer> DummyDevice::CreateCommandBuffer()
    {
        return { new DummyCommandBuffer() };
    }

    Common::UniqueRef<Fence> DummyDevice::CreateFence(const bool bInitAsSignaled)
    {
        return { new DummyFence(*this, bInitAsSignaled) };
    }

    Common::UniqueRef<Semaphore> DummyDevice::CreateSemaphore()
    {
        return { new DummySemaphore(*this) };
    }

    bool DummyDevice::CheckSwapChainFormatSupport(Surface* surface, PixelFormat format)
    {
        return true;
    }
}
