//
// Created by John Kindem on 2021/4/30.
//

#ifndef EXPLOSION_SIGNAL_H
#define EXPLOSION_SIGNAL_H

#include <vulkan/vulkan.h>

namespace Explosion::RHI {
    class Driver;
    class Device;

    class Signal {
    public:
        explicit Signal(Driver& driver);
        ~Signal();
        const VkSemaphore& GetVkSemaphore();

    private:
        void CreateSemaphore();
        void DestroySemaphore();

        Driver& driver;
        Device& device;
        VkSemaphore vkSemaphore = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_SIGNAL_H
