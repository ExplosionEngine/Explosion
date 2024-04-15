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
        
        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        Common::UniqueRef<Surface> CreateSurface(const RHI::SurfaceCreateInfo &createInfo) override;
        Common::UniqueRef<SwapChain> CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        Common::UniqueRef<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Common::UniqueRef<Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Common::UniqueRef<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Common::UniqueRef<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override;
        Common::UniqueRef<BindGroup> CreateBindGroup(const BindGroupCreateInfo& createInfo) override;
        Common::UniqueRef<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        Common::UniqueRef<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        Common::UniqueRef<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Common::UniqueRef<RasterPipeline> CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo) override;
        Common::UniqueRef<CommandBuffer> CreateCommandBuffer() override;
        Common::UniqueRef<Fence> CreateFence(bool bInitAsSignaled) override;
        Common::UniqueRef<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface *surface, PixelFormat format) override;

    private:
        Common::UniqueRef<DummyQueue> dummyQueue;
    };
}
