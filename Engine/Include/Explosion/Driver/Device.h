//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DEVICE_H
#define EXPLOSION_DEVICE_H

#include <vulkan/vulkan.h>

namespace Explosion {
    class Device {
    public:
        Device();
        ~Device();

    private:
        void PrepareExtensions();
        void CreateInstance();
        void DestroyInstance();

        std::vector<const char*> extensions {};
        VkInstance vkInstance = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_DEVICE_H
