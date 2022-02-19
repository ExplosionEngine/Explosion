//
// Created by johnk on 16/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_DEVICE_H
#define EXPLOSION_RHI_VULKAN_DEVICE_H

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
        void Destroy() override;
        Buffer* CreateBuffer(const BufferCreateInfo* createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo* createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo* createInfo) override;
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo) override;
        BindGroup* CreateBindGroup(const BindGroupCreateInfo* createInfo) override;
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo) override;
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo* createInfo) override;

        vk::Device GetVkDevice();

    private:
        static std::optional<uint32_t> FindQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& properties, std::vector<uint32_t>& usedQueueFamily, QueueType queueType);
        void CreateDevice(VKGpu& gpu, const DeviceCreateInfo* createInfo);
        void GetQueues();

        vk::Device vkDevice;
        std::unordered_map<QueueType, std::pair<uint32_t, uint32_t>> queueFamilyMappings;
        std::unordered_map<QueueType, std::vector<std::unique_ptr<VKQueue>>> queues;
    };
}

#endif //EXPLOSION_RHI_VULKAN_DEVICE_H
