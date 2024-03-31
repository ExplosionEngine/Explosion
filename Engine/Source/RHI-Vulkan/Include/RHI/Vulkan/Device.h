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
    class VulkanGpu;
    class VulkanQueue;

    class VulkanDevice : public Device {
    public:
        NonCopyable(VulkanDevice)
        VulkanDevice(VulkanGpu& inGpu, const DeviceCreateInfo& inCreateInfo);
        ~VulkanDevice() override;

        void Destroy() override;
        size_t GetQueueNum(QueueType inType) override;
        Handle<Queue> GetQueue(QueueType inType, size_t inIndex) override;
        Holder<Surface> CreateSurface(const SurfaceCreateInfo& inCreateInfo) override;
        Holder<SwapChain> CreateSwapChain(const SwapChainCreateInfo& inCreateInfo) override;
        Holder<Buffer> CreateBuffer(const BufferCreateInfo& inCreateInfo) override;
        Holder<Texture> CreateTexture(const TextureCreateInfo& inCreateInfo) override;
        Holder<Sampler> CreateSampler(const SamplerCreateInfo& inCreateInfo) override;
        Holder<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo) override;
        Holder<BindGroup> CreateBindGroup(const BindGroupCreateInfo& inCreateInfo) override;
        Holder<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo) override;
        Holder<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo) override;
        Holder<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo) override;
        Holder<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& inCreateInfo) override;
        Holder<CommandBuffer> CreateCommandBuffer() override;
        Holder<Fence> CreateFence(bool initAsSignaled) override;
        Holder<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Handle<Surface> inSurface, PixelFormat inFormat) override;

        VkDevice GetNative();
        VmaAllocator& GetNativeAllocator();
        VulkanGpu& GetGpu() const;

#if BUILD_CONFIG_DEBUG
        void SetObjectName(VkObjectType inObjectType, uint64_t inObjectHandle, const char* inObjectName);
#endif

    private:
        static std::optional<uint32_t> FindQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& inProperties, std::vector<uint32_t>& inUsedQueueFamily, QueueType inQueueType);
        void CreateNativeDevice(const DeviceCreateInfo& inCreateInfo);
        void GetQueues();
        void CreateNativeVmaAllocator();

        VulkanGpu& gpu;
        VkDevice nativeDevice;
        VmaAllocator nativeAllocator;
        std::unordered_map<QueueType, std::pair<uint32_t, uint32_t>> queueFamilyMappings;
        std::unordered_map<QueueType, std::vector<Common::UniqueRef<VulkanQueue>>> queues;
        std::unordered_map<QueueType, VkCommandPool> nativeCmdPools;
    };
}
