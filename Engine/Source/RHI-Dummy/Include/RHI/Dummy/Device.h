//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Device.h>

namespace RHI::Dummy {
    class DummyQueue;

    class DummyDevice final : public Device {
    public:
        NonCopyable(DummyDevice)
        explicit DummyDevice(const DeviceCreateInfo& createInfo);
        ~DummyDevice() override;
        
        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        Common::UniquePtr<Surface> CreateSurface(const RHI::SurfaceCreateInfo &createInfo) override;
        Common::UniquePtr<SwapChain> CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        Common::UniquePtr<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Common::UniquePtr<Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Common::UniquePtr<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Common::UniquePtr<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override;
        Common::UniquePtr<BindGroup> CreateBindGroup(const BindGroupCreateInfo& createInfo) override;
        Common::UniquePtr<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        Common::UniquePtr<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        Common::UniquePtr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Common::UniquePtr<RasterPipeline> CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo) override;
        Common::UniquePtr<CommandBuffer> CreateCommandBuffer() override;
        Common::UniquePtr<Fence> CreateFence(bool bInitAsSignaled) override;
        Common::UniquePtr<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface *surface, PixelFormat format) override;
        TextureSubResourceCopyFootprint GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo) override;

    private:
        Common::UniquePtr<DummyQueue> dummyQueue;
    };
}
