//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/RHI/Vulkan/VulkanDriver.h>

namespace Explosion::RHI {
    VulkanDriver::VulkanDriver() : device(std::make_unique<VulkanDevice>(*this)) {}

    VulkanDriver::~VulkanDriver() = default;

    VulkanDevice* VulkanDriver::GetDevice()
    {
        return device.get();
    }
}
