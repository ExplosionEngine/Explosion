//
// Created by johnk on 16/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_DEVICE_H
#define EXPLOSION_RHI_VULKAN_DEVICE_H

#include <optional>

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

    private:
        static std::optional<uint32_t> FindQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& properties, QueueType queueType);
        void CreateDevice(VKGpu& gpu, const DeviceCreateInfo* createInfo);
        void GetQueues();

        vk::Device vkDevice;
        std::unordered_map<QueueType, std::vector<std::unique_ptr<VKQueue>>> queues;
    };
}

#endif //EXPLOSION_RHI_VULKAN_DEVICE_H
