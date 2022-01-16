//
// Created by johnk on 16/1/2022.
//

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Queue.h>

namespace RHI::Vulkan {
    VKDevice::VKDevice(VKGpu& gpu, const DeviceCreateInfo* createInfo) : Device(createInfo)
    {
        CreateDevice(gpu, createInfo);
    }

    VKDevice::~VKDevice()
    {
        vkDevice.destroy();
    }

    size_t VKDevice::GetQueueNum(QueueType type)
    {
        auto iter = queues.find(type);
        if (iter == queues.end()) {
            throw VKException("failed to find specific queue family");
        }
        return iter->second.size();
    }

    Queue* VKDevice::GetQueue(QueueType type, size_t index)
    {
        auto iter = queues.find(type);
        if (iter == queues.end()) {
            throw VKException("failed to find specific queue family");
        }
        auto& queueArray = iter->second;
        if (index < 0 || index >= queueArray.size()) {
            throw VKException("bad queue index");
        }
        return queueArray[index].get();
    }

    void VKDevice::Destroy()
    {
        delete this;
    }

    std::optional<uint32_t> VKDevice::FindQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& properties, QueueType queueType)
    {
        for (uint32_t i = 0; i < properties.size(); i++) {
            if (properties[i].queueFlags & VKEnumCast<QueueType, vk::QueueFlagBits>(queueType)) {
                return i;
            }
        }
        return {};
    }

    void VKDevice::CreateDevice(VKGpu& gpu, const DeviceCreateInfo* createInfo)
    {
        uint32_t queueFamilyPropertyCnt = 0;
        gpu.GetVkPhysicalDevice().getQueueFamilyProperties(&queueFamilyPropertyCnt, nullptr);
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCnt);
        gpu.GetVkPhysicalDevice().getQueueFamilyProperties(&queueFamilyPropertyCnt, queueFamilyProperties.data());

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(createInfo->queueCreateInfoNum);
        for (auto i = 0; i < queueCreateInfos.size(); i++) {
            auto queueFamilyIndex = FindQueueFamilyIndex(queueFamilyProperties, createInfo->queueCreateInfos[i].type);
            if (!queueFamilyIndex.has_value()) {
                throw VKException("failed to found suitable queue family");
            }
            queueCreateInfos[i].queueFamilyIndex = queueFamilyIndex.value();
            queueCreateInfos[i].queueCount = createInfo->queueCreateInfos[i].num;
        }

        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        if (gpu.GetVkPhysicalDevice().createDevice(&deviceCreateInfo, nullptr, &vkDevice) != vk::Result::eSuccess) {
            throw VKException("failed to create device");
        }
    }

    void VKDevice::GetQueues()
    {
        // TODO
    }
}
