//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_COMMANDBUFFER_H
#define EXPLOSION_COMMANDBUFFER_H

#include <functional>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Driver;
    class Device;
    class CommandEncoder;

    using EncodingFunc = std::function<void(CommandEncoder* commandEncoder)>;

    class CommandBuffer {
    public:
        explicit CommandBuffer(Driver& driver);
        ~CommandBuffer();
        const VkCommandBuffer& GetVkCommandBuffer();
        void EncodeCommands(const EncodingFunc& encodingFunc);

    private:
        void AllocateCommandBuffer();
        void FreeCommandBuffer();

        Driver& driver;
        Device& device;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_COMMANDBUFFER_H
