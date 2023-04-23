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
        return type == QueueType::GRAPHICS ? 1 : 0;
    }

    Queue* DummyDevice::GetQueue(QueueType type, size_t index)
    {
        Assert(type == QueueType::GRAPHICS && index == 0);
        return dummyQueue.Get();
    }

    Surface* DummyDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return new DummySurface(createInfo);
    }

    SwapChain* DummyDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        return new DummySwapChain(createInfo);
    }

    Buffer* DummyDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return new DummyBuffer(createInfo);
    }

    Texture* DummyDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return new DummyTexture(createInfo);
    }

    Sampler* DummyDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return new DummySampler(createInfo);
    }

    BindGroupLayout* DummyDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        return new DummyBindGroupLayout(createInfo);
    }

    BindGroup* DummyDevice::CreateBindGroup(const BindGroupCreateInfo& createInfo)
    {
        return new DummyBindGroup(createInfo);
    }

    PipelineLayout* DummyDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        return new DummyPipelineLayout(createInfo);
    }

    ShaderModule* DummyDevice::CreateShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        return new DummyShaderModule(createInfo);
    }

    ComputePipeline* DummyDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return new DummyComputePipeline(createInfo);
    }

    GraphicsPipeline* DummyDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        return new DummyGraphicsPipeline(createInfo);
    }

    CommandBuffer* DummyDevice::CreateCommandBuffer()
    {
        return new DummyCommandBuffer();
    }

    Fence* DummyDevice::CreateFence()
    {
        return new DummyFence(*this);
    }

    bool DummyDevice::CheckSwapChainFormatSupport(Surface* surface, PixelFormat format)
    {
        return true;
    }
}
