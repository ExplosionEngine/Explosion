//
// Created by John Kindem on 2021/1/9 0009.
//

#ifndef EXPLOSION_VULKAN_DRIVER_H
#define EXPLOSION_VULKAN_DRIVER_H

#include <memory>

#include <driver/driver.h>
#include <driver/vulkan/vulkan-context.h>

namespace Explosion {
    class VulkanDriver : public Driver {
    public:
        VulkanDriver();
        ~VulkanDriver() override;

    private:
        std::unique_ptr<VulkanContext> device;
    };
}

#endif //EXPLOSION_VULKAN_DRIVER_H
