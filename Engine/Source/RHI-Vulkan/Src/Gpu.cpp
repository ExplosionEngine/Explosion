//
// Created by johnk on 12/1/2022.
//

#include <algorithm>

#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>

namespace RHI::Vulkan {
    VulkanGpu::VulkanGpu(VulkanInstance& inInstance, VkPhysicalDevice inNativePhysicalDevice)
        : instance(inInstance)
        , nativePhysicalDevice(inNativePhysicalDevice)
    {
    }

    VulkanGpu::~VulkanGpu() = default;

    GpuProperty VulkanGpu::GetProperty()
    {
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(nativePhysicalDevice, &vkPhysicalDeviceProperties);

        GpuProperty property {};
        property.vendorId = vkPhysicalDeviceProperties.vendorID;
        property.deviceId = vkPhysicalDeviceProperties.deviceID;
        property.type = EnumCast<VkPhysicalDeviceType, GpuType>(vkPhysicalDeviceProperties.deviceType);
        return property;
    }

    FeatureFlags VulkanGpu::GetFeatures()
    {
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(nativePhysicalDevice, &features);

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(nativePhysicalDevice, &properties);

        FeatureFlags result = FeatureFlags::null;
        if (features.samplerAnisotropy) { result = result | FeatureBits::samplerAnisotropy; }
        if (features.textureCompressionBC) { result = result | FeatureBits::textureCompressionBc; }
        if (properties.limits.timestampComputeAndGraphics) { result = result | FeatureBits::timestampQuery; }
        if (features.multiDrawIndirect) { result = result | FeatureBits::multiDrawIndirect; }
        if (features.drawIndirectFirstInstance) { result = result | FeatureBits::drawIndirectFirstInstance; }
        return result;
    }

    GpuLimits VulkanGpu::GetLimits()
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(nativePhysicalDevice, &properties);
        const auto& nativeLimits = properties.limits;

        // Vulkan 1.0 has no direct max-buffer-size query, so the largest device-local heap is used as an approximation.
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(nativePhysicalDevice, &memoryProperties);
        VkDeviceSize maxDeviceLocalHeapSize = 0;
        for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) {
            if ((memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0) {
                maxDeviceLocalHeapSize = std::max(maxDeviceLocalHeapSize, memoryProperties.memoryHeaps[i].size);
            }
        }

        GpuLimits result {};
        result.maxTextureDimension1D = nativeLimits.maxImageDimension1D;
        result.maxTextureDimension2D = nativeLimits.maxImageDimension2D;
        result.maxTextureDimension3D = nativeLimits.maxImageDimension3D;
        result.maxTextureArrayLayers = nativeLimits.maxImageArrayLayers;
        result.maxBindGroups = nativeLimits.maxBoundDescriptorSets;
        result.maxVertexBuffers = nativeLimits.maxVertexInputBindings;
        result.maxVertexAttributes = nativeLimits.maxVertexInputAttributes;
        result.maxColorAttachments = nativeLimits.maxColorAttachments;
        result.maxUniformBufferBindingSize = nativeLimits.maxUniformBufferRange;
        result.maxStorageBufferBindingSize = nativeLimits.maxStorageBufferRange;
        result.maxBufferSize = maxDeviceLocalHeapSize;
        result.minUniformBufferOffsetAlignment = static_cast<uint32_t>(nativeLimits.minUniformBufferOffsetAlignment);
        result.minStorageBufferOffsetAlignment = static_cast<uint32_t>(nativeLimits.minStorageBufferOffsetAlignment);
        result.optimalBufferCopyOffsetAlignment = static_cast<uint32_t>(nativeLimits.optimalBufferCopyOffsetAlignment);
        result.optimalBufferCopyRowPitchAlignment = static_cast<uint32_t>(nativeLimits.optimalBufferCopyRowPitchAlignment);
        result.maxComputeWorkgroupSizeX = nativeLimits.maxComputeWorkGroupSize[0];
        result.maxComputeWorkgroupSizeY = nativeLimits.maxComputeWorkGroupSize[1];
        result.maxComputeWorkgroupSizeZ = nativeLimits.maxComputeWorkGroupSize[2];
        result.maxComputeInvocationsPerWorkgroup = nativeLimits.maxComputeWorkGroupInvocations;
        result.maxComputeWorkgroupsPerDimension = nativeLimits.maxComputeWorkGroupCount[0];
        return result;
    }

    Common::UniquePtr<Device> VulkanGpu::RequestDevice(const DeviceCreateInfo& inCreateInfo)
    {
        return { new VulkanDevice(*this, inCreateInfo) };
    }

    VulkanInstance& VulkanGpu::GetInstance() const
    {
        return instance;
    }

    VkPhysicalDevice VulkanGpu::GetNative() const
    {
        return nativePhysicalDevice;
    }

    uint32_t VulkanGpu::FindMemoryType(uint32_t inFilter, VkMemoryPropertyFlags inPropertyFlag) const
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(nativePhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((inFilter & 1 << i) != 0u && (memProperties.memoryTypes[i].propertyFlags & inPropertyFlag) == inPropertyFlag) {
                return i;
            }
        }
        Assert(false && "failed to found suitable memory type");
        // compile warning, has no effects
        return -1;
    }
}
