//
// Created by johnk on 12/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_GRAPHICS_CARD_H
#define EXPLOSION_RHI_DX12_GRAPHICS_CARD_H

#include <vulkan/vulkan.hpp>

#include <RHI/Gpu.h>

namespace RHI::Vulkan {
    class VKGpu : public Gpu {
    public:
        NON_COPYABLE(VKGpu)
        explicit VKGpu(vk::PhysicalDevice vkPhysicalDevice);
        ~VKGpu() override;
        const GpuProperty& GetProperty() override;

    private:
        void GetPhysicalDeviceProperties();

        GpuProperty property;
        vk::PhysicalDeviceProperties vkPhysicalDeviceProperties;
        vk::PhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties;
        vk::PhysicalDevice vkPhysicalDevice;
    };
}

#endif //EXPLOSION_RHI_DX12_GRAPHICS_CARD_H
