//
// Created by johnk on 16/1/2022.
//

#include <map>
#include <algorithm>

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/SwapChain.h>
#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    const std::vector<const char*> DEVICE_EXTENSIONS = {
        "VK_KHR_swapchain",
        "VK_KHR_dynamic_rendering",
        "VK_KHR_depth_stencil_resolve",
        "VK_KHR_create_renderpass2",
#ifdef __APPLE__
        "VK_KHR_portability_subset"
#endif
    };

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };


    VKDevice::VKDevice(VKGpu& vkGpu, const DeviceCreateInfo* createInfo) : Device(createInfo), gpu(vkGpu)
    {
        CreateDevice(createInfo);
        GetQueues();
    }

    VKDevice::~VKDevice()
    {
        for (auto& [queue, pool] : pools) {
            vkDevice.destroyCommandPool(pool);
        }
        vkDevice.destroy();
    }

    size_t VKDevice::GetQueueNum(QueueType type)
    {
        auto iter = queues.find(type);
        Assert(iter != queues.end());
        return iter->second.size();
    }

    Queue* VKDevice::GetQueue(QueueType type, size_t index)
    {
        auto iter = queues.find(type);
        Assert(iter != queues.end());
        auto& queueArray = iter->second;
        Assert(index < queueArray.size());
        return queueArray[index].get();
    }

    SwapChain* VKDevice::CreateSwapChain(const SwapChainCreateInfo* createInfo)
    {
        return new VKSwapChain(*this, createInfo);
    }

    void VKDevice::Destroy()
    {
        delete this;
    }

    Buffer* VKDevice::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        return new VKBuffer(*this, createInfo);
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
        return new VKBindGroupLayout(*this, createInfo);
    }

    BindGroup* VKDevice::CreateBindGroup(const BindGroupCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    PipelineLayout* VKDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
    {
        return new VKPipelineLayout(*this, createInfo);
    }

    ShaderModule* VKDevice::CreateShaderModule(const ShaderModuleCreateInfo* createInfo)
    {
        return new VKShaderModule(*this, createInfo);
    }

    ComputePipeline* VKDevice::CreateComputePipeline(const ComputePipelineCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    GraphicsPipeline* VKDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
    {
        return new VKGraphicsPipeline(*this, createInfo);
    }

    CommandBuffer* VKDevice::CreateCommandBuffer()
    {
        return new VKCommandBuffer(*this, pools[QueueType::GRAPHICS]);
    }

    Fence* VKDevice::CreateFence()
    {
        return new VKFence(*this);
    }

    vk::Device VKDevice::GetVkDevice()
    {
        return vkDevice;
    }

    VKGpu& VKDevice::GetGpu() const
    {
        return gpu;
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

    void VKDevice::CreateDevice(const DeviceCreateInfo* createInfo)
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
            Assert(queueFamilyIndex.has_value());
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

        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
        dynamicRenderingFeatures.setDynamicRendering(VK_TRUE);
        deviceCreateInfo.pNext = &dynamicRenderingFeatures;

        deviceCreateInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());

#ifdef BUILD_CONFIG_DEBUG
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        deviceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

        Assert(gpu.GetVkPhysicalDevice().createDevice(&deviceCreateInfo, nullptr, &vkDevice) == vk::Result::eSuccess);
    }

    void VKDevice::GetQueues()
    {
        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        for (auto iter : queueFamilyMappings) {
            auto queueType = iter.first;
            auto queueFamilyIndex = iter.second.first;
            auto queueNum = iter.second.second;

            std::vector<std::unique_ptr<VKQueue>> tempQueues(queueNum);
            for (auto i = 0; i < tempQueues.size(); i++) {
                tempQueues[i] = std::make_unique<VKQueue>(vkDevice.getQueue(queueFamilyIndex, i));
            }
            queues[queueType] = std::move(tempQueues);

            poolInfo.setQueueFamilyIndex(iter.second.first);

            vk::CommandPool pool;
            Assert(vkDevice.createCommandPool(&poolInfo, nullptr, &pool) == vk::Result::eSuccess);
            pools.emplace(queueType, pool);
        }
    }
}
