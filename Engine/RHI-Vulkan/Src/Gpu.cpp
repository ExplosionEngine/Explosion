//
// Created by johnk on 12/1/2022.
//

#include <RHI/Vulkan/Gpu.h>

namespace RHI::Vulkan {
    VKGpu::VKGpu(vk::PhysicalDevice d) : Gpu(), vkPhysicalDevice(d), property({})
    {
        GetPhysicalDeviceProperties();
    }

    VKGpu::~VKGpu() = default;

    const GpuProperty& VKGpu::GetProperty()
    {
        return property;
    }

    void VKGpu::GetPhysicalDeviceProperties()
    {
        vkPhysicalDevice.getProperties(&vkPhysicalDeviceProperties);
        vkPhysicalDevice.getMemoryProperties(&vkPhysicalDeviceMemoryProperties);

        // TODO
    }
}
