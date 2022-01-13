//
// Created by johnk on 12/1/2022.
//

#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VKGpu::VKGpu(vk::PhysicalDevice d) : Gpu(), vkPhysicalDevice(d) {}

    VKGpu::~VKGpu() = default;

    GpuProperty VKGpu::GetProperty()
    {
        vk::PhysicalDeviceProperties vkPhysicalDeviceProperties;
        vkPhysicalDevice.getProperties(&vkPhysicalDeviceProperties);
        vk::PhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties;
        vkPhysicalDevice.getMemoryProperties(&vkPhysicalDeviceMemoryProperties);

        GpuProperty property {};
        property.vendorId = vkPhysicalDeviceProperties.vendorID;
        property.deviceId = vkPhysicalDeviceProperties.deviceID;
        property.type = VKEnumCast<vk::PhysicalDeviceType, GpuType>(vkPhysicalDeviceProperties.deviceType);
        return property;
    }
}
