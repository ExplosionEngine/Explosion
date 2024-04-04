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
        Queue* GetQueue(QueueType type, size_t index) override;
        Surface* CreateSurface(const RHI::SurfaceCreateInfo &createInfo) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        Buffer* CreateBuffer(const BufferCreateInfo& createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo& createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo& createInfo) override;
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override;
        BindGroup* CreateBindGroup(const BindGroupCreateInfo& createInfo) override;
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        RasterPipeline* CreateRasterPipeline(const RasterPipelineCreateInfo& createInfo) override;
        CommandBuffer* CreateCommandBuffer() override;
        Fence* CreateFence(bool bInitAsSignaled) override;
        Semaphore* CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface *surface, PixelFormat format) override;

    private:
        Common::UniqueRef<DummyQueue> dummyQueue;
    };
}
