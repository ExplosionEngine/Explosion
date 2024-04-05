//
// Created by johnk on 12/1/2022.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Gpu.h>

namespace RHI::Vulkan {
    class VulkanInstance;

    class VulkanGpu : public Gpu {
    public:
        NonCopyable(VulkanGpu)
        explicit VulkanGpu(VulkanInstance& inInstance, VkPhysicalDevice inNativePhysicalDevice);
        ~VulkanGpu() override;

        GpuProperty GetProperty() override;
        Common::UniqueRef<Device> RequestDevice(const DeviceCreateInfo& inCreateInfo) override;

        VkPhysicalDevice GetNative() const;
        VulkanInstance& GetInstance() const;
        uint32_t FindMemoryType(uint32_t inFilter, VkMemoryPropertyFlags inPropertyFlag) const;

    private:
        VulkanInstance& instance;
        VkPhysicalDevice nativePhysicalDevice;
    };
}
