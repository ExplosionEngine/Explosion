//
// Created by Zach Lee on 2022/9/22.
//

#pragma once

#include <RHI/Device.h>
#include <unordered_map>
#include <vector>
#include <memory>

#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLGpu;
    class MTLQueue;

    class MTLDevice : public Device {
    public:
        NON_COPYABLE(MTLDevice)

        MTLDevice(MTLGpu& gpu, const DeviceCreateInfo& createInfo);
        ~MTLDevice() override;

        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        Buffer* CreateBuffer(const BufferCreateInfo& createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo& createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo& createInfo) override { return nullptr; }
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override { return nullptr; }
        BindGroup* CreateBindGroup(const BindGroupCreateInfo& createInfo) override { return nullptr; }
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override { return nullptr; }
        GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        CommandBuffer* CreateCommandBuffer() override;
        Fence* CreateFence() override;

        void Destroy() override;

        MTLGpu& GetGpu() const;

        id<MTLDevice> GetDevice() const;

        MTLSharedEventListener *GetSharedEventListener() const;

    private:
        MTLGpu &gpu;
        id<MTLDevice> mtlDevice = nil;
        MTLSharedEventListener *listener = nil;
        std::unordered_map<QueueType, std::vector<Common::UniqueRef<MTLQueue>>> queues;
    };
}
