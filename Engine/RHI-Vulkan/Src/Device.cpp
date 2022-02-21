//
// Created by johnk on 16/1/2022.
//

#include <map>
#include <algorithm>

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Queue.h>

namespace RHI::Vulkan {
    const std::vector<const char*> DEVICE_EXTENSIONS = {
        "VK_KHR_swapchain",
#ifdef __APPLE__
        "VK_KHR_portability_subset"
#endif
    };

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };


    VKDevice::VKDevice(VKGpu& gpu, const DeviceCreateInfo* createInfo) : Device(createInfo)
    {
        CreateDevice(gpu, createInfo);
        GetQueues();
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
            throw VKException("failed to find queue with specific type");
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

    Buffer* VKDevice::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    Texture* VKDevice::CreateTexture(const TextureCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    Sampler* VKDevice::CreateSampler(const SamplerCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    BindGroupLayout* VKDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    BindGroup* VKDevice::CreateBindGroup(const BindGroupCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    PipelineLayout* VKDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    ShaderModule* VKDevice::CreateShaderModule(const ShaderModuleCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    ComputePipeline* VKDevice::CreateComputePipeline(const ComputePipelineCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    GraphicsPipeline* VKDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    CommandBuffer* VKDevice::CreateCommandBuffer(const CommandBufferCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    vk::Device VKDevice::GetVkDevice()
    {
        return vkDevice;
    }

    std::optional<uint32_t> VKDevice::FindQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& properties, std::vector<uint32_t>& usedQueueFamily, QueueType queueType)
    {
        for (uint32_t i = 0; i < properties.size(); i++) {
            auto iter = std::find(usedQueueFamily.begin(), usedQueueFamily.end(), i);
            if (iter != usedQueueFamily.end()) {
                continue;
            }

            if (properties[i].queueFlags & VKEnumCast<QueueType, vk::QueueFlagBits>(queueType)) {
                usedQueueFamily.emplace_back(i);
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

        std::map<QueueType, uint32_t> queueNumMap;
        for (uint32_t i = 0; i < createInfo->queueCreateInfoNum; i++) {
            const auto& queueCreateInfo = createInfo->queueCreateInfos[i];
            auto iter = queueNumMap.find(queueCreateInfo.type);
            if (iter == queueNumMap.end()) {
                queueNumMap[queueCreateInfo.type] = 0;
            }
            queueNumMap[queueCreateInfo.type] += queueCreateInfo.num;
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::vector<uint32_t> usedQueueFamily;
        std::vector<float> queuePriorities;
        for (auto iter : queueNumMap) {
            auto queueFamilyIndex = FindQueueFamilyIndex(queueFamilyProperties, usedQueueFamily, iter.first);
            if (!queueFamilyIndex.has_value()) {
                throw VKException("failed to found suitable queue family");
            }
            auto queueCount = std::min(queueFamilyProperties[queueFamilyIndex.value()].queueCount, iter.second);

            if (queueCount > queuePriorities.size()) {
                queuePriorities.resize(queueCount, 1.0f);
            }

            vk::DeviceQueueCreateInfo tempCreateInfo {};
            tempCreateInfo.setQueueFamilyIndex(queueFamilyIndex.value())
                .setQueueCount(queueCount)
                .setPQueuePriorities(queuePriorities.data());
            queueCreateInfos.emplace_back(tempCreateInfo);

            queueFamilyMappings[iter.first] = std::make_pair(queueFamilyIndex.value(), queueCount);
        }

        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        deviceCreateInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());

#ifdef BUILD_CONFIG_DEBUG
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        deviceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

        if (gpu.GetVkPhysicalDevice().createDevice(&deviceCreateInfo, nullptr, &vkDevice) != vk::Result::eSuccess) {
            throw VKException("failed to create device");
        }
    }

    void VKDevice::GetQueues()
    {
        for (auto iter : queueFamilyMappings) {
            auto queueType = iter.first;
            auto queueFamilyIndex = iter.second.first;
            auto queueNum = iter.second.second;

            std::vector<std::unique_ptr<VKQueue>> tempQueues(queueNum);
            for (auto i = 0; i < tempQueues.size(); i++) {
                tempQueues[i] = std::make_unique<VKQueue>(vkDevice.getQueue(queueFamilyIndex, i));
            }
            queues[queueType] = std::move(tempQueues);
        }
    }
}
