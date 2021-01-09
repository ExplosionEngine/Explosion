//
// Created by Administrator on 2021/1/9 0009.
//

#ifndef EXPLOSION_VULKAN_DRIVER_H
#define EXPLOSION_VULKAN_DRIVER_H

#include <memory>

#include <driver/driver.h>
#include <driver/vulkan/vulkan-device.h>

namespace Explosion {
    class VulkanDriver : public Driver {
    public:
        VulkanDriver();
        ~VulkanDriver() override;
        Device* GetDevice() override;

    private:
        std::unique_ptr<VulkanDevice> device;
    };
}

#endif //EXPLOSION_VULKAN_DRIVER_H
