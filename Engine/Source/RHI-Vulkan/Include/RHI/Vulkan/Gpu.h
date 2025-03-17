//
// Created by johnk on 12/1/2022.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Gpu.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    class VulkanGpu final : public Gpu {
    public:
        NonCopyable(VulkanGpu)
        explicit VulkanGpu(VulkanInstance& inInstance, VkPhysicalDevice inNativePhysicalDevice);
        ~VulkanGpu() override;

        GpuProperty GetProperty() override;
        Common::UniquePtr<Device> RequestDevice(const DeviceCreateInfo& inCreateInfo) override;
        VulkanInstance& GetInstance() const override;

        VkPhysicalDevice GetNative() const;
        uint32_t FindMemoryType(uint32_t inFilter, VkMemoryPropertyFlags inPropertyFlag) const;

    private:
        VulkanInstance& instance;
        VkPhysicalDevice nativePhysicalDevice;
    };
}
