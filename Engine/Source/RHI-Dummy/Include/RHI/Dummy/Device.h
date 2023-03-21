//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <memory>

#include <RHI/Device.h>

namespace RHI::Dummy {
    class DummyQueue;

    class DummyDevice : public Device {
    public:
        explicit DummyDevice(const DeviceCreateInfo* createInfo);
        ~DummyDevice() override;
        
        void Destroy() override;
        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo* createInfo) override;
        Buffer* CreateBuffer(const BufferCreateInfo* createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo* createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo* createInfo) override;
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo) override;
        BindGroup* CreateBindGroup(const BindGroupCreateInfo* createInfo) override;
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo) override;
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo* createInfo) override;
        ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo* createInfo) override;
        GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo) override;
        CommandBuffer* CreateCommandBuffer() override;
        Fence* CreateFence() override;

    private:
        std::unique_ptr<DummyQueue> dummyQueue;
    };
}
