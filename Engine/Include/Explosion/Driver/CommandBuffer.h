//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_COMMANDBUFFER_H
#define EXPLOSION_COMMANDBUFFER_H

#include <vulkan/vulkan.h>

namespace Explosion {
    class Driver;
    class Device;

    class CommandBuffer {
    public:
        CommandBuffer(Driver& driver);
        ~CommandBuffer();
        const VkCommandBuffer& GetVkCommandBuffer();

    private:
        void AllocateCommandBuffer();
        void FreeCommandBuffer();

        Driver& driver;
        Device& device;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_COMMANDBUFFER_H
