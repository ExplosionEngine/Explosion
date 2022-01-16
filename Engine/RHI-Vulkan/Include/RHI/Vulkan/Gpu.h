//
// Created by johnk on 12/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_GPU_H
#define EXPLOSION_RHI_VULKAN_GPU_H

#include <vulkan/vulkan.hpp>

#include <RHI/Gpu.h>

namespace RHI::Vulkan {
    class VKGpu : public Gpu {
    public:
        NON_COPYABLE(VKGpu)
        explicit VKGpu(vk::PhysicalDevice vkPhysicalDevice);
        ~VKGpu() override;

        GpuProperty GetProperty() override;
        Device* RequestDevice(const DeviceCreateInfo* createInfo) override;

        vk::PhysicalDevice GetVkPhysicalDevice();

    private:
        vk::PhysicalDevice vkPhysicalDevice;
    };
}

#endif //EXPLOSION_RHI_VULKAN_GPU_H
