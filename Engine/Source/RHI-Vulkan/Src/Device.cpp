//
// Created by johnk on 16/1/2022.
//

#include <map>
#include <algorithm>

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/BindGroup.h>
#include <RHI/Vulkan/Sampler.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/SwapChain.h>
#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Synchronous.h>
#include <RHI/Vulkan/Surface.h>

namespace RHI::Vulkan {
    const std::vector<const char*> DEVICE_EXTENSIONS = {
        "VK_KHR_swapchain",
        "VK_KHR_dynamic_rendering",
        "VK_KHR_depth_stencil_resolve",
        "VK_KHR_create_renderpass2",
#if PLATFORM_MACOS
        "VK_KHR_portability_subset"
#endif
    };

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };


    VKDevice::VKDevice(VKGpu& vkGpu, const DeviceCreateInfo& createInfo) : Device(createInfo), gpu(vkGpu)
    {
        CreateDevice(createInfo);
        GetQueues();
        CreateVmaAllocator();
    }

    VKDevice::~VKDevice()
    {
        vmaDestroyAllocator(vmaAllocator);

        for (auto& [queue, pool] : pools) {
            vkDestroyCommandPool(vkDevice, pool, nullptr);
        }
        vkDestroyDevice(vkDevice, nullptr);
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
        return queueArray[index].Get();
    }

    Surface* VKDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return new VKSurface(*this, createInfo);
    }

    SwapChain* VKDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        return new VKSwapChain(*this, createInfo);
    }

    void VKDevice::Destroy()
    {
        delete this;
    }

    Buffer* VKDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return new VKBuffer(*this, createInfo);
    }

    Texture* VKDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return new VKTexture(*this, createInfo);
    }

    Sampler* VKDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return new VKSampler(*this, createInfo);
    }

    BindGroupLayout* VKDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        return new VKBindGroupLayout(*this, createInfo);
    }

    BindGroup* VKDevice::CreateBindGroup(const BindGroupCreateInfo& createInfo)
    {
        return new VKBindGroup(*this, createInfo);
    }

    PipelineLayout* VKDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        return new VKPipelineLayout(*this, createInfo);
    }

    ShaderModule* VKDevice::CreateShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        return new VKShaderModule(*this, createInfo);
    }

    ComputePipeline* VKDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        // TODO
        return nullptr;
    }

    GraphicsPipeline* VKDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        return new VKGraphicsPipeline(*this, createInfo);
    }

    CommandBuffer* VKDevice::CreateCommandBuffer()
    {
        return new VKCommandBuffer(*this, pools[QueueType::graphics]);
    }

    Fence* VKDevice::CreateFence(bool initAsSignaled)
    {
        return new VKFence(*this, initAsSignaled);
    }

    Semaphore* VKDevice::CreateSemaphore()
    {
        return new VKSemaphore(*this);
    }

    bool VKDevice::CheckSwapChainFormatSupport(Surface* surface, PixelFormat format)
    {
        auto* vkSurface = static_cast<VKSurface*>(surface);
        VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        uint32_t formatCount = 0;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.GetVkPhysicalDevice(), vkSurface->GetVKSurface(), &formatCount, nullptr);
        Assert(formatCount != 0);
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.GetVkPhysicalDevice(), vkSurface->GetVKSurface(), &formatCount, surfaceFormats.data());

        auto iter = std::find_if(
            surfaceFormats.begin(),
            surfaceFormats.end(),
            [format = VKEnumCast<PixelFormat, VkFormat>(format), colorSpace](VkSurfaceFormatKHR surfaceFormat) {
                return format == surfaceFormat.format && colorSpace == surfaceFormat.colorSpace;
            });
        return iter != surfaceFormats.end();
    }

    VkDevice VKDevice::GetVkDevice()
    {
        return vkDevice;
    }

    VKGpu& VKDevice::GetGpu() const
    {
        return gpu;
    }

    std::optional<uint32_t> VKDevice::FindQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& properties, std::vector<uint32_t>& usedQueueFamily, QueueType queueType)
    {
        for (uint32_t i = 0; i < properties.size(); i++) {
            auto iter = std::find(usedQueueFamily.begin(), usedQueueFamily.end(), i);
            if (iter != usedQueueFamily.end()) {
                continue;
            }

            if (properties[i].queueFlags & VKEnumCast<QueueType, VkQueueFlagBits>(queueType)) {
                usedQueueFamily.emplace_back(i);
                return i;
            }
        }
        return {};
    }

    void VKDevice::CreateDevice(const DeviceCreateInfo& createInfo)
    {
        uint32_t queueFamilyPropertyCnt = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.GetVkPhysicalDevice(), &queueFamilyPropertyCnt, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCnt);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.GetVkPhysicalDevice(), &queueFamilyPropertyCnt, queueFamilyProperties.data());

        std::map<QueueType, uint32_t> queueNumMap;
        for (uint32_t i = 0; i < createInfo.queueRequests.size(); i++) {
            const auto& queueCreateInfo = createInfo.queueRequests[i];
            auto iter = queueNumMap.find(queueCreateInfo.type);
            if (iter == queueNumMap.end()) {
                queueNumMap[queueCreateInfo.type] = 0;
            }
            queueNumMap[queueCreateInfo.type] += queueCreateInfo.num;
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<uint32_t> usedQueueFamily;
        std::vector<float> queuePriorities;
        for (auto iter : queueNumMap) {
            auto queueFamilyIndex = FindQueueFamilyIndex(queueFamilyProperties, usedQueueFamily, iter.first);
            Assert(queueFamilyIndex.has_value());
            auto queueCount = std::min(queueFamilyProperties[queueFamilyIndex.value()].queueCount, iter.second);

            if (queueCount > queuePriorities.size()) {
                queuePriorities.resize(queueCount, 1.0f);
            }

            VkDeviceQueueCreateInfo tempCreateInfo = {};
            tempCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            tempCreateInfo.queueFamilyIndex = queueFamilyIndex.value();
            tempCreateInfo.queueCount = queueCount;
            tempCreateInfo.pQueuePriorities = queuePriorities.data();
            queueCreateInfos.emplace_back(tempCreateInfo);

            queueFamilyMappings[iter.first] = std::make_pair(queueFamilyIndex.value(), queueCount);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
        dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
        deviceCreateInfo.pNext = &dynamicRenderingFeatures;

        deviceCreateInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());

#ifdef BUILD_CONFIG_DEBUG
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        deviceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

        Assert(vkCreateDevice(gpu.GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &vkDevice) == VK_SUCCESS);
    }

    void VKDevice::GetQueues()
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        for (auto iter : queueFamilyMappings) {
            auto queueType = iter.first;
            auto queueFamilyIndex = iter.second.first;
            auto queueNum = iter.second.second;

            std::vector<Common::UniqueRef<VKQueue>> tempQueues(queueNum);
            for (auto i = 0; i < tempQueues.size(); i++) {
                VkQueue queue;
                vkGetDeviceQueue(vkDevice, queueFamilyIndex, i, &queue);
                tempQueues[i] = Common::MakeUnique<VKQueue>(*this, queue);
            }
            queues[queueType] = std::move(tempQueues);

            poolInfo.queueFamilyIndex = iter.second.first;

            VkCommandPool pool;
            Assert(vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &pool) == VK_SUCCESS);
            pools.emplace(queueType, pool);
        }
    }

    void VKDevice::CreateVmaAllocator()
    {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo info = {};
        info.vulkanApiVersion = VK_API_VERSION_1_3;
        info.instance = gpu.GetInstance().GetVkInstance();
        info.physicalDevice = gpu.GetVkPhysicalDevice();
        info.device = vkDevice;
        info.pVulkanFunctions = &vulkanFunctions;

        vmaCreateAllocator(&info, &vmaAllocator);
    }

    VmaAllocator& VKDevice::GetVmaAllocator()
    {
        return vmaAllocator;
    }

#if BUILD_CONFIG_DEBUG
    void VKDevice::SetObjectName(VkObjectType objectType, uint64_t objectHandle, const char* objectName)
    {
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectType                    = objectType;
        info.objectHandle                  = objectHandle;
        info.pObjectName                   = objectName;

        gpu.GetInstance().vkSetDebugUtilsObjectNameEXT(vkDevice, &info);
    }
#endif
}
