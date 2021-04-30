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
    class Signal;

    using EncodingFunc = std::function<void(CommandEncoder* commandEncoder)>;

    class CommandBuffer {
    public:
        explicit CommandBuffer(Driver& driver);
        ~CommandBuffer();
        const VkCommandBuffer& GetVkCommandBuffer();
        void EncodeCommands(const EncodingFunc& encodingFunc);
        void SubmitNow();
        void Submit(Signal* waitSignal, Signal* notifySignal);

    protected:
        virtual void SetupSubmitInfo(VkSubmitInfo& submitInfo);

        Driver& driver;
        Device& device;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

    private:
        void AllocateCommandBuffer();
        void FreeCommandBuffer();
    };

    class FrameOutputCommandBuffer : public CommandBuffer {
    public:
        explicit FrameOutputCommandBuffer(Driver& driver);
        ~FrameOutputCommandBuffer();

    protected:
        void SetupSubmitInfo(VkSubmitInfo& submitInfo) override;
    };
}

#endif //EXPLOSION_COMMANDBUFFER_H
