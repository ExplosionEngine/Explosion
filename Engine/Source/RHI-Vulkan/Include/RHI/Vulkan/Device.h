//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <optional>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <RHI/Device.h>

namespace RHI::Vulkan {
    class VKGpu;
    class VKQueue;

    class VKDevice : public Device {
    public:
        NonCopyable(VKDevice)
        VKDevice(VKGpu& gpu, const DeviceCreateInfo& createInfo);
        ~VKDevice() override;

        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        Surface* CreateSurface(const RHI::SurfaceCreateInfo &createInfo) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        void Destroy() override;
        Buffer* CreateBuffer(const BufferCreateInfo& createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo& createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo& createInfo) override;
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override;
        BindGroup* CreateBindGroup(const BindGroupCreateInfo& createInfo) override;
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        CommandBuffer* CreateCommandBuffer() override;
        Fence* CreateFence(bool initAsSignaled) override;
        Semaphore* CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface* surface, PixelFormat format) override;

        VkDevice GetVkDevice();
        VKGpu& GetGpu() const;
        VmaAllocator& GetVmaAllocator();

#if BUILD_CONFIG_DEBUG
        void SetObjectName(VkObjectType objectType, uint64_t objectHandle, const char* objectName);
#endif

    private:
        static std::optional<uint32_t> FindQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& properties, std::vector<uint32_t>& usedQueueFamily, QueueType queueType);
        void CreateDevice(const DeviceCreateInfo& createInfo);
        void GetQueues();
        void CreateVmaAllocator();

        VKGpu& gpu;
        VkDevice vkDevice;
        VmaAllocator vmaAllocator;
        std::unordered_map<QueueType, std::pair<uint32_t, uint32_t>> queueFamilyMappings;
        std::unordered_map<QueueType, std::vector<Common::UniqueRef<VKQueue>>> queues;
        std::unordered_map<QueueType, VkCommandPool> pools;
    };
}
