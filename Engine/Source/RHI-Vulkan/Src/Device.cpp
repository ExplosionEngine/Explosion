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
}

namespace RHI::Vulkan {
    VulkanDevice::VulkanDevice(VulkanGpu& inGpu, const DeviceCreateInfo& inCreateInfo)
        : Device(inCreateInfo)
        , gpu(inGpu)
    {
        CreateNativeDevice(inCreateInfo);
        GetQueues();
        CreateNativeVmaAllocator();
    }

    VulkanDevice::~VulkanDevice()
    {
        vmaDestroyAllocator(nativeAllocator);

        for (auto& [queue, pool] : nativeCmdPools) {
            vkDestroyCommandPool(nativeDevice, pool, nullptr);
        }
        vkDestroyDevice(nativeDevice, nullptr);
    }

    size_t VulkanDevice::GetQueueNum(QueueType inType)
    {
        auto iter = queues.find(inType);
        Assert(iter != queues.end());
        return iter->second.size();
    }

    Queue* VulkanDevice::GetQueue(QueueType inType, size_t inIndex)
    {
        auto iter = queues.find(inType);
        Assert(iter != queues.end());
        auto& queueArray = iter->second;
        Assert(inIndex < queueArray.size());
        return queueArray[inIndex].Get();
    }

    Common::UniqueRef<Surface> VulkanDevice::CreateSurface(const SurfaceCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<Surface>(new VulkanSurface(*this, inCreateInfo));
    }

    Common::UniqueRef<SwapChain> VulkanDevice::CreateSwapChain(const SwapChainCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<SwapChain>(new VulkanSwapChain(*this, inCreateInfo));
    }

    Common::UniqueRef<Buffer> VulkanDevice::CreateBuffer(const BufferCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<Buffer>(new VulkanBuffer(*this, inCreateInfo));
    }

    Common::UniqueRef<Texture> VulkanDevice::CreateTexture(const TextureCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<Texture>(new VulkanTexture(*this, inCreateInfo));
    }

    Common::UniqueRef<Sampler> VulkanDevice::CreateSampler(const SamplerCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<Sampler>(new VulkanSampler(*this, inCreateInfo));
    }

    Common::UniqueRef<BindGroupLayout> VulkanDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<BindGroupLayout>(new VulkanBindGroupLayout(*this, inCreateInfo));
    }

    Common::UniqueRef<BindGroup> VulkanDevice::CreateBindGroup(const BindGroupCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<BindGroup>(new VulkanBindGroup(*this, inCreateInfo));
    }

    Common::UniqueRef<PipelineLayout> VulkanDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<PipelineLayout>(new VulkanPipelineLayout(*this, inCreateInfo));
    }

    Common::UniqueRef<ShaderModule> VulkanDevice::CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<ShaderModule>(new VulkanShaderModule(*this, inCreateInfo));
    }

    Common::UniqueRef<ComputePipeline> VulkanDevice::CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo)
    {
        // TODO
        return nullptr;
    }

    Common::UniqueRef<RasterPipeline> VulkanDevice::CreateRasterPipeline(const RasterPipelineCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<RasterPipeline>(new VulkanRasterPipeline(*this, inCreateInfo));
    }

    Common::UniqueRef<CommandBuffer> VulkanDevice::CreateCommandBuffer()
    {
        return Common::UniqueRef<CommandBuffer>(new VulkanCommandBuffer(*this, nativeCmdPools[QueueType::graphics]));
    }

    Common::UniqueRef<Fence> VulkanDevice::CreateFence(bool initAsSignaled)
    {
        return Common::UniqueRef<Fence>(new VulkanFence(*this, initAsSignaled));
    }

    Common::UniqueRef<Semaphore> VulkanDevice::CreateSemaphore()
    {
        return Common::UniqueRef<Semaphore>(new VulkanSemaphore(*this));
    }

    bool VulkanDevice::CheckSwapChainFormatSupport(Surface* inSurface, PixelFormat inFormat)
    {
        auto* vkSurface = static_cast<VulkanSurface*>(inSurface);
        VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        uint32_t formatCount = 0;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.GetNative(), vkSurface->GetNative(), &formatCount, nullptr);
        Assert(formatCount != 0);
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.GetNative(), vkSurface->GetNative(), &formatCount, surfaceFormats.data());

        auto iter = std::find_if(
            surfaceFormats.begin(),
            surfaceFormats.end(),
            [format = EnumCast<PixelFormat, VkFormat>(inFormat), colorSpace](VkSurfaceFormatKHR surfaceFormat) {
                return format == surfaceFormat.format && colorSpace == surfaceFormat.colorSpace;
            });
        return iter != surfaceFormats.end();
    }

    VkDevice VulkanDevice::GetNative()
    {
        return nativeDevice;
    }

    VulkanGpu& VulkanDevice::GetGpu() const
    {
        return gpu;
    }

    std::optional<uint32_t> VulkanDevice::FindQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& inProperties, std::vector<uint32_t>& inUsedQueueFamily, QueueType inQueueType)
    {
        for (uint32_t i = 0; i < inProperties.size(); i++) {
            auto iter = std::find(inUsedQueueFamily.begin(), inUsedQueueFamily.end(), i);
            if (iter != inUsedQueueFamily.end()) {
                continue;
            }

            if (inProperties[i].queueFlags & EnumCast<QueueType, VkQueueFlagBits>(inQueueType)) {
                inUsedQueueFamily.emplace_back(i);
                return i;
            }
        }
        return {};
    }

    void VulkanDevice::CreateNativeDevice(const DeviceCreateInfo& inCreateInfo)
    {
        uint32_t queueFamilyPropertyCnt = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.GetNative(), &queueFamilyPropertyCnt, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCnt);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.GetNative(), &queueFamilyPropertyCnt, queueFamilyProperties.data());

        std::map<QueueType, uint32_t> queueNumMap;
        for (uint32_t i = 0; i < inCreateInfo.queueRequests.size(); i++) {
            const auto& queueCreateInfo = inCreateInfo.queueRequests[i];
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

        Assert(vkCreateDevice(gpu.GetNative(), &deviceCreateInfo, nullptr, &nativeDevice) == VK_SUCCESS);
    }

    void VulkanDevice::GetQueues()
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        for (auto iter : queueFamilyMappings) {
            auto queueType = iter.first;
            auto queueFamilyIndex = iter.second.first;
            auto queueNum = iter.second.second;

            std::vector<Common::UniqueRef<VulkanQueue>> tempQueues(queueNum);
            for (auto i = 0; i < tempQueues.size(); i++) {
                VkQueue queue;
                vkGetDeviceQueue(nativeDevice, queueFamilyIndex, i, &queue);
                tempQueues[i] = Common::MakeUnique<VulkanQueue>(*this, queue);
            }
            queues[queueType] = std::move(tempQueues);

            poolInfo.queueFamilyIndex = iter.second.first;

            VkCommandPool pool;
            Assert(vkCreateCommandPool(nativeDevice, &poolInfo, nullptr, &pool) == VK_SUCCESS);
            nativeCmdPools.emplace(queueType, pool);
        }
    }

    void VulkanDevice::CreateNativeVmaAllocator()
    {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo info = {};
        info.vulkanApiVersion = VK_API_VERSION_1_3;
        info.instance = gpu.GetInstance().GetNative();
        info.physicalDevice = gpu.GetNative();
        info.device = nativeDevice;
        info.pVulkanFunctions = &vulkanFunctions;

        vmaCreateAllocator(&info, &nativeAllocator);
    }

    VmaAllocator& VulkanDevice::GetNativeAllocator()
    {
        return nativeAllocator;
    }

#if BUILD_CONFIG_DEBUG
    void VulkanDevice::SetObjectName(VkObjectType inObjectType, uint64_t inObjectHandle, const char* inObjectName)
    {
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectType                    = inObjectType;
        info.objectHandle                  = inObjectHandle;
        info.pObjectName                   = inObjectName;

        gpu.GetInstance().pfnVkSetDebugUtilsObjectNameEXT(nativeDevice, &info);
    }
#endif
}
