//
// Created by johnk on 12/1/2022.
//

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

    Common::UniqueRef<Device> VulkanGpu::RequestDevice(const DeviceCreateInfo& inCreateInfo)
    {
        return { new VulkanDevice(*this, inCreateInfo) };
    }

    VkPhysicalDevice VulkanGpu::GetNative() const
    {
        return nativePhysicalDevice;
    }

    VulkanInstance& VulkanGpu::GetInstance() const
    {
        return instance;
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
