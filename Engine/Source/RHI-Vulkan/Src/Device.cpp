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
    const std::vector requiredExtensions = {
        "VK_KHR_swapchain",
        "VK_KHR_dynamic_rendering",
        "VK_KHR_depth_stencil_resolve",
        "VK_KHR_create_renderpass2",
#if PLATFORM_MACOS
        "VK_KHR_portability_subset",
        "VK_EXT_extended_dynamic_state"
#endif
    };

    const std::vector requiredValidationLayers = {
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

    size_t VulkanDevice::GetQueueNum(const QueueType inType)
    {
        return queues.at(inType).size();
    }

    Queue* VulkanDevice::GetQueue(QueueType inType, size_t inIndex)
    {
        const auto& queueArray = queues.at(inType);
        Assert(inIndex < queueArray.size());
        return queueArray[inIndex].Get();
    }

    Common::UniquePtr<Surface> VulkanDevice::CreateSurface(const SurfaceCreateInfo& inCreateInfo)
    {
        return { new VulkanSurface(*this, inCreateInfo) };
    }

    Common::UniquePtr<SwapChain> VulkanDevice::CreateSwapChain(const SwapChainCreateInfo& inCreateInfo)
    {
        return { new VulkanSwapChain(*this, inCreateInfo) };
    }

    Common::UniquePtr<Buffer> VulkanDevice::CreateBuffer(const BufferCreateInfo& inCreateInfo)
    {
        return { new VulkanBuffer(*this, inCreateInfo) };
    }

    Common::UniquePtr<Texture> VulkanDevice::CreateTexture(const TextureCreateInfo& inCreateInfo)
    {
        return { new VulkanTexture(*this, inCreateInfo) };
    }

    Common::UniquePtr<Sampler> VulkanDevice::CreateSampler(const SamplerCreateInfo& inCreateInfo)
    {
        return { new VulkanSampler(*this, inCreateInfo) };
    }

    Common::UniquePtr<BindGroupLayout> VulkanDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo)
    {
        return { new VulkanBindGroupLayout(*this, inCreateInfo) };
    }

    Common::UniquePtr<BindGroup> VulkanDevice::CreateBindGroup(const BindGroupCreateInfo& inCreateInfo)
    {
        return { new VulkanBindGroup(*this, inCreateInfo) };
    }

    Common::UniquePtr<PipelineLayout> VulkanDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo)
    {
        return { new VulkanPipelineLayout(*this, inCreateInfo) };
    }

    Common::UniquePtr<ShaderModule> VulkanDevice::CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo)
    {
        return { new VulkanShaderModule(*this, inCreateInfo) };
    }

    Common::UniquePtr<ComputePipeline> VulkanDevice::CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo)
    {
        return { new VulkanComputePipeline(*this, inCreateInfo) };
    }

    Common::UniquePtr<RasterPipeline> VulkanDevice::CreateRasterPipeline(const RasterPipelineCreateInfo& inCreateInfo)
    {
        return { new VulkanRasterPipeline(*this, inCreateInfo) };
    }

    Common::UniquePtr<CommandBuffer> VulkanDevice::CreateCommandBuffer()
    {
        return { new VulkanCommandBuffer(*this, nativeCmdPools[QueueType::graphics]) };
    }

    Common::UniquePtr<Fence> VulkanDevice::CreateFence(const bool initAsSignaled)
    {
        return { new VulkanFence(*this, initAsSignaled) };
    }

    Common::UniquePtr<Semaphore> VulkanDevice::CreateSemaphore()
    {
        return { new VulkanSemaphore(*this) };
    }

    bool VulkanDevice::CheckSwapChainFormatSupport(Surface* inSurface, const PixelFormat inFormat)
    {
        const auto* vkSurface = static_cast<VulkanSurface*>(inSurface);
        VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        uint32_t formatCount = 0;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.GetNative(), vkSurface->GetNative(), &formatCount, nullptr);
        Assert(formatCount != 0);
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.GetNative(), vkSurface->GetNative(), &formatCount, surfaceFormats.data());

        const auto iter = std::ranges::find_if(
            surfaceFormats,
            [format = EnumCast<PixelFormat, VkFormat>(inFormat), colorSpace](const VkSurfaceFormatKHR surfaceFormat) {
                return format == surfaceFormat.format && colorSpace == surfaceFormat.colorSpace;
            });
        return iter != surfaceFormats.end();
    }

    TextureSubResourceCopyFootprint VulkanDevice::GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo)
    {
        const auto& vkTexture = static_cast<const VulkanTexture&>(texture);
        const auto& createInfo = texture.GetCreateInfo();

        VkImageSubresource subResource {};
        subResource.mipLevel = subResourceInfo.mipLevel;
        subResource.arrayLayer = subResourceInfo.arrayLayer;
        subResource.aspectMask = EnumCast<TextureAspect, VkImageAspectFlags>(subResourceInfo.aspect);

        TextureSubResourceCopyFootprint result {};
        result.extent = { createInfo.width, createInfo.height, createInfo.dimension == TextureDimension::t3D ? createInfo.depthOrArraySize : 1 };
        result.bytesPerPixel = GetBytesPerPixel(createInfo.format);
        result.rowPitch = result.bytesPerPixel * result.extent.x;
        result.slicePitch = result.rowPitch * result.extent.y;
        result.totalBytes = result.bytesPerPixel * result.extent.x * result.extent.y * result.extent.z;
        return result;
    }

    VkDevice VulkanDevice::GetNative() const
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
            if (const auto iter = std::ranges::find(inUsedQueueFamily, i);
                iter != inUsedQueueFamily.end()) {
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
            if (auto iter = queueNumMap.find(queueCreateInfo.type);
                iter == queueNumMap.end()) {
                queueNumMap[queueCreateInfo.type] = 0;
            }
            queueNumMap[queueCreateInfo.type] += queueCreateInfo.num;
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<uint32_t> usedQueueFamily;
        std::vector<float> queuePriorities;
        for (auto [queueType, queueNum] : queueNumMap) {
            auto queueFamilyIndex = FindQueueFamilyIndex(queueFamilyProperties, usedQueueFamily, queueType);
            Assert(queueFamilyIndex.has_value());
            auto queueCount = std::min(queueFamilyProperties[queueFamilyIndex.value()].queueCount, queueNum);

            if (queueCount > queuePriorities.size()) {
                queuePriorities.resize(queueCount, 1.0f);
            }

            VkDeviceQueueCreateInfo tempCreateInfo = {};
            tempCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            tempCreateInfo.queueFamilyIndex = queueFamilyIndex.value();
            tempCreateInfo.queueCount = queueCount;
            tempCreateInfo.pQueuePriorities = queuePriorities.data();
            queueCreateInfos.emplace_back(tempCreateInfo);

            queueFamilyMappings[queueType] = std::make_pair(queueFamilyIndex.value(), queueCount);
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

#if PLATFORM_MACOS
        // MoltenVK not support use vkCmdSetPrimitiveTopology() directly current
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures = {};
        extendedDynamicStateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
        extendedDynamicStateFeatures.extendedDynamicState = VK_TRUE;
        dynamicRenderingFeatures.pNext = &extendedDynamicStateFeatures;
#endif

        deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());

#if BUILD_CONFIG_DEBUG
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#endif

        Assert(vkCreateDevice(gpu.GetNative(), &deviceCreateInfo, nullptr, &nativeDevice) == VK_SUCCESS);
    }

    void VulkanDevice::GetQueues()
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        for (auto [queueType, queueFamilyInfo] : queueFamilyMappings) {
            auto [queueFamilyIndex, queueNum] = queueFamilyInfo;

            std::vector<Common::UniquePtr<VulkanQueue>> tempQueues(queueNum);
            for (auto i = 0; i < tempQueues.size(); i++) {
                VkQueue queue;
                vkGetDeviceQueue(nativeDevice, queueFamilyIndex, i, &queue);
                tempQueues[i] = Common::MakeUnique<VulkanQueue>(*this, queue);
            }
            queues[queueType] = std::move(tempQueues);

            poolInfo.queueFamilyIndex = queueFamilyIndex;

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
    void VulkanDevice::SetObjectName(const VkObjectType inObjectType, const uint64_t inObjectHandle, const char* inObjectName) const
    {
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectType                    = inObjectType;
        info.objectHandle                  = inObjectHandle;
        info.pObjectName                   = inObjectName;

        auto* pfn = gpu.GetInstance().FindOrGetTypedDynamicFuncPointer<PFN_vkSetDebugUtilsObjectNameEXT>("vkSetDebugUtilsObjectNameEXT");
        pfn(nativeDevice, &info);
    }
#endif
}
