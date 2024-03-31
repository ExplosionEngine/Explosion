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

    void DummyDevice::Destroy()
    {
        delete this;
    }

    size_t DummyDevice::GetQueueNum(QueueType type)
    {
        return type == QueueType::graphics ? 1 : 0;
    }

    Handle<Queue> DummyDevice::GetQueue(QueueType type, size_t index)
    {
        Assert(type == QueueType::graphics && index == 0);
        return dummyQueue.Get();
    }

    Holder<Surface> DummyDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return Common::UniqueRef<Surface>(new DummySurface(createInfo));
    }

    Holder<SwapChain> DummyDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        return Common::UniqueRef<SwapChain>(new DummySwapChain(createInfo));
    }

    Holder<Buffer> DummyDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return Common::UniqueRef<Buffer>(new DummyBuffer(createInfo));
    }

    Holder<Texture> DummyDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return Common::UniqueRef<Texture>(new DummyTexture(createInfo));
    }

    Holder<Sampler> DummyDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return Common::UniqueRef<Sampler>(new DummySampler(createInfo));
    }

    Holder<BindGroupLayout> DummyDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        return Common::UniqueRef<BindGroupLayout>(new DummyBindGroupLayout(createInfo));
    }

    Holder<BindGroup> DummyDevice::CreateBindGroup(const BindGroupCreateInfo& createInfo)
    {
        return Common::UniqueRef<BindGroup>(new DummyBindGroup(createInfo));
    }

    Holder<PipelineLayout> DummyDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        return Common::UniqueRef<PipelineLayout>(new DummyPipelineLayout(createInfo));
    }

    Holder<ShaderModule> DummyDevice::CreateShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        return Common::UniqueRef<ShaderModule>(new DummyShaderModule(createInfo));
    }

    Holder<ComputePipeline> DummyDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return Common::UniqueRef<ComputePipeline>(new DummyComputePipeline(createInfo));
    }

    Holder<GraphicsPipeline> DummyDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        return Common::UniqueRef<GraphicsPipeline>(new DummyGraphicsPipeline(createInfo));
    }

    Holder<CommandBuffer> DummyDevice::CreateCommandBuffer()
    {
        return Common::UniqueRef<CommandBuffer>(new DummyCommandBuffer());
    }

    Holder<Fence> DummyDevice::CreateFence(bool bInitAsSignaled)
    {
        return Common::UniqueRef<Fence>(new DummyFence(*this, bInitAsSignaled));
    }

    Holder<Semaphore> DummyDevice::CreateSemaphore()
    {
        return Common::UniqueRef<Semaphore>(new DummySemaphore(*this));
    }

    bool DummyDevice::CheckSwapChainFormatSupport(Handle<Surface> surface, PixelFormat format)
    {
        return true;
    }
}
