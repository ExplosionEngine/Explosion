//
// Created by johnk on 12/1/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include <RHI/Gpu.h>

namespace RHI::Vulkan {
    class VKGpu : public Gpu {
    public:
        NON_COPYABLE(VKGpu)
        explicit VKGpu(vk::Instance instance, vk::PhysicalDevice vkPhysicalDevice);
        ~VKGpu() override;

        GpuProperty GetProperty() override;
        Device* RequestDevice(const DeviceCreateInfo* createInfo) override;

        const vk::PhysicalDevice& GetVkPhysicalDevice() const;

        const vk::Instance& GetVKInstance() const;

        uint32_t FindMemoryType(uint32_t filter, vk::MemoryPropertyFlags propertyFlag) const;

    private:
        vk::Instance vkInstance;
        vk::PhysicalDevice vkPhysicalDevice;
    };
}
