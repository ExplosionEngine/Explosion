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

    size_t DummyDevice::GetQueueNum(QueueType type)
    {
        return type == QueueType::graphics ? 1 : 0;
    }

    Queue* DummyDevice::GetQueue(QueueType type, size_t index)
    {
        Assert(type == QueueType::graphics && index == 0);
        return dummyQueue.Get();
    }

    Common::UniqueRef<Surface> DummyDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return Common::UniqueRef<Surface>(new DummySurface(createInfo));
    }

    Common::UniqueRef<SwapChain> DummyDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        return Common::UniqueRef<SwapChain>(new DummySwapChain(createInfo));
    }

    Common::UniqueRef<Buffer> DummyDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return Common::UniqueRef<Buffer>(new DummyBuffer(createInfo));
    }

    Common::UniqueRef<Texture> DummyDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return Common::UniqueRef<Texture>(new DummyTexture(createInfo));
    }

    Common::UniqueRef<Sampler> DummyDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return Common::UniqueRef<Sampler>(new DummySampler(createInfo));
    }

    Common::UniqueRef<BindGroupLayout> DummyDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        return Common::UniqueRef<BindGroupLayout>(new DummyBindGroupLayout(createInfo));
    }

    Common::UniqueRef<BindGroup> DummyDevice::CreateBindGroup(const BindGroupCreateInfo& createInfo)
    {
        return Common::UniqueRef<BindGroup>(new DummyBindGroup(createInfo));
    }

    Common::UniqueRef<PipelineLayout> DummyDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        return Common::UniqueRef<PipelineLayout>(new DummyPipelineLayout(createInfo));
    }

    Common::UniqueRef<ShaderModule> DummyDevice::CreateShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        return Common::UniqueRef<ShaderModule>(new DummyShaderModule(createInfo));
    }

    Common::UniqueRef<ComputePipeline> DummyDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return Common::UniqueRef<ComputePipeline>(new DummyComputePipeline(createInfo));
    }

    Common::UniqueRef<RasterPipeline> DummyDevice::CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo)
    {
        return Common::UniqueRef<RasterPipeline>(new DummyRasterPipeline(createInfo));
    }

    Common::UniqueRef<CommandBuffer> DummyDevice::CreateCommandBuffer()
    {
        return Common::UniqueRef<CommandBuffer>(new DummyCommandBuffer());
    }

    Common::UniqueRef<Fence> DummyDevice::CreateFence(bool bInitAsSignaled)
    {
        return Common::UniqueRef<Fence>(new DummyFence(*this, bInitAsSignaled));
    }

    Common::UniqueRef<Semaphore> DummyDevice::CreateSemaphore()
    {
        return Common::UniqueRef<Semaphore>(new DummySemaphore(*this));
    }

    bool DummyDevice::CheckSwapChainFormatSupport(Surface* surface, PixelFormat format)
    {
        return true;
    }
}
