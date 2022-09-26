//
// Created by Zach Lee on 2022/9/22.
//

#pragma once

#include <RHI/Device.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLGpu;

    class MTLDevice : public Device {
    public:
        NON_COPYABLE(MTLDevice)

        MTLDevice(MTLGpu& gpu, const DeviceCreateInfo* createInfo);
        ~MTLDevice() override;

        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override { return nullptr; }
        SwapChain* CreateSwapChain(const SwapChainCreateInfo* createInfo) override { return nullptr; }
        Buffer* CreateBuffer(const BufferCreateInfo* createInfo) override { return nullptr; }
        Texture* CreateTexture(const TextureCreateInfo* createInfo) override { return nullptr; }
        Sampler* CreateSampler(const SamplerCreateInfo* createInfo) override { return nullptr; }
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo) override { return nullptr; }
        BindGroup* CreateBindGroup(const BindGroupCreateInfo* createInfo) override { return nullptr; }
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo) override { return nullptr; }
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo* createInfo) override { return nullptr; }
        ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo* createInfo) override { return nullptr; }
        GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo) override { return nullptr; }
        CommandBuffer* CreateCommandBuffer() override { return nullptr; }
        Fence* CreateFence() override { return nullptr; }

        void Destroy() override;

        MTLGpu& GetGpu() const;

    private:
        MTLGpu &gpu;
        id<MTLDevice> mtlDevice = nil;
    };
}