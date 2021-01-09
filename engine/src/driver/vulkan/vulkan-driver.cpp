//
// Created by Administrator on 2021/1/9 0009.
//

#include <driver/vulkan/vulkan-driver.h>

Explosion::VulkanDriver::VulkanDriver()
    : device(std::make_unique<VulkanDevice>()) {

}

Explosion::VulkanDriver::~VulkanDriver() {

}

Explosion::Device* Explosion::VulkanDriver::GetDevice() {
    return device.get();
}
