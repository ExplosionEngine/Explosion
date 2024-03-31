//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Device.h>

namespace RHI::Dummy {
    class DummyQueue;

    class DummyDevice : public Device {
    public:
        NonCopyable(DummyDevice)
        explicit DummyDevice(const DeviceCreateInfo& createInfo);
        ~DummyDevice() override;
        
        void Destroy() override;
        size_t GetQueueNum(QueueType type) override;
        Handle<Queue> GetQueue(QueueType type, size_t index) override;
        Holder<Surface> CreateSurface(const SurfaceCreateInfo& createInfo) override;
        Holder<SwapChain> CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        Holder<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Holder<Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Holder<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Holder<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override;
        Holder<BindGroup> CreateBindGroup(const BindGroupCreateInfo& createInfo) override;
        Holder<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        Holder<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        Holder<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Holder<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        Holder<CommandBuffer> CreateCommandBuffer() override;
        Holder<Fence> CreateFence(bool bInitAsSignaled) override;
        Holder<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Handle<Surface> surface, PixelFormat format) override;

    private:
        Common::UniqueRef<DummyQueue> dummyQueue;
    };
}
