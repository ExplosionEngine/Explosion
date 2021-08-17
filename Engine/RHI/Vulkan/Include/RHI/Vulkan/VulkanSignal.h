//
// Created by John Kindem on 2021/4/30.
//

#ifndef EXPLOSION_VULKANSIGNAL_H
#define EXPLOSION_VULKANSIGNAL_H

#include <vulkan/vulkan.h>

#include <RHI/Signal.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class VulkanSignal : public Signal {
    public:
        explicit VulkanSignal(VulkanDriver& driver);
        ~VulkanSignal() override;
        const VkSemaphore& GetVkSemaphore();

    private:
        void CreateSignal();
        void DestroySignal();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkSemaphore vkSemaphore = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANSIGNAL_H
