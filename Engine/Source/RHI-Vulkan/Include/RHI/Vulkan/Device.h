//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <RHI/Device.h>
#include <RHI/Vulkan/Gpu.h>

namespace RHI::Vulkan {
    class VulkanQueue;

    class VulkanDevice final : public Device {
    public:
        NonCopyable(VulkanDevice)
        VulkanDevice(VulkanGpu& inGpu, const DeviceCreateInfo& inCreateInfo);
        ~VulkanDevice() override;

        VulkanGpu& GetGpu() const override;
        size_t GetQueueNum(QueueType inType) override;
        Queue* GetQueue(QueueType inType, size_t inIndex) override;
        Common::UniquePtr<Surface> CreateSurface(const SurfaceCreateInfo& inCreateInfo) override;
        Common::UniquePtr<SwapChain> CreateSwapChain(const SwapChainCreateInfo& inCreateInfo) override;
        Common::UniquePtr<Buffer> CreateBuffer(const BufferCreateInfo& inCreateInfo) override;
        Common::UniquePtr<Texture> CreateTexture(const TextureCreateInfo& inCreateInfo) override;
        Common::UniquePtr<Sampler> CreateSampler(const SamplerCreateInfo& inCreateInfo) override;
        Common::UniquePtr<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo) override;
        Common::UniquePtr<BindGroup> CreateBindGroup(const BindGroupCreateInfo& inCreateInfo) override;
        Common::UniquePtr<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo) override;
        Common::UniquePtr<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo) override;
        Common::UniquePtr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo) override;
        Common::UniquePtr<RasterPipeline> CreateRasterPipeline(const RasterPipelineCreateInfo& inCreateInfo) override;
        Common::UniquePtr<CommandBuffer> CreateCommandBuffer() override;
        Common::UniquePtr<Fence> CreateFence(bool initAsSignaled) override;
        Common::UniquePtr<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface* inSurface, PixelFormat inFormat) override;
        TextureSubResourceCopyFootprint GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo) override;

        VkDevice GetNative() const;
        VmaAllocator& GetNativeAllocator();

#if BUILD_CONFIG_DEBUG
        void SetObjectName(VkObjectType inObjectType, uint64_t inObjectHandle, const char* inObjectName) const;
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
        std::unordered_map<QueueType, std::vector<Common::UniquePtr<VulkanQueue>>> queues;
        std::unordered_map<QueueType, VkCommandPool> nativeCmdPools;
    };
}
