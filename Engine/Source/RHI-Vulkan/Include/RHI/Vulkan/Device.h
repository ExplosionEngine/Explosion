//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <optional>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include <RHI/Device.h>

namespace RHI::Vulkan {
    class VKGpu;
    class VKQueue;

    class VKDevice : public Device {
    public:
        NON_COPYABLE(VKDevice)
        VKDevice(VKGpu& gpu, const DeviceCreateInfo* createInfo);
        ~VKDevice() override;

        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo* createInfo) override;
        void Destroy() override;
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

        vk::Device GetVkDevice();

        VKGpu* GetGpu() const;

    private:
        static std::optional<uint32_t> FindQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& properties, std::vector<uint32_t>& usedQueueFamily, QueueType queueType);
        void CreateDevice(const DeviceCreateInfo* createInfo);
        void GetQueues();

        VKGpu* gpu;
        vk::Device vkDevice;
        std::unordered_map<QueueType, std::pair<uint32_t, uint32_t>> queueFamilyMappings;
        std::unordered_map<QueueType, std::vector<std::unique_ptr<VKQueue>>> queues;
    };
}
