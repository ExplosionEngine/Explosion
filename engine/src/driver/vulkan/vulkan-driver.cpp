//
// Created by John Kindem on 2021/1/9 0009.
//

#include <driver/vulkan/vulkan-driver.h>

namespace Explosion {
    VulkanDriver::VulkanDriver() : device(std::make_unique<VulkanContext>()) {}

    VulkanDriver::~VulkanDriver() = default;
}