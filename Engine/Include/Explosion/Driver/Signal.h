//
// Created by John Kindem on 2021/4/30.
//

#ifndef EXPLOSION_SIGNAL_H
#define EXPLOSION_SIGNAL_H

#include <vulkan/vulkan.h>

#include <Explosion/Driver/GpuRes.h>

namespace Explosion {
    class Driver;
    class Device;

    class Signal : public GpuRes {
    public:
        explicit Signal(Driver& driver);
        ~Signal() override;
        const VkSemaphore& GetVkSemaphore();

    private:
        void OnCreate() override;
        void OnDestroy() override;
        void CreateSemaphore();
        void DestroySemaphore();

        Device& device;
        VkSemaphore vkSemaphore = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_SIGNAL_H
