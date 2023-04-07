//
// Created by johnk on 12/1/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include <RHI/Gpu.h>

namespace RHI::Vulkan {
    class VKInstance;

    class VKGpu : public Gpu {
    public:
        NON_COPYABLE(VKGpu)
        explicit VKGpu(VKInstance& instance, vk::PhysicalDevice vkPhysicalDevice);
        ~VKGpu() override;

        GpuProperty GetProperty() override;
        Device* RequestDevice(const DeviceCreateInfo& createInfo) override;

        const vk::PhysicalDevice& GetVkPhysicalDevice() const;

        VKInstance& GetInstance() const;

        uint32_t FindMemoryType(uint32_t filter, vk::MemoryPropertyFlags propertyFlag) const;

    private:
        VKInstance& instance;
        vk::PhysicalDevice vkPhysicalDevice;
    };
}
