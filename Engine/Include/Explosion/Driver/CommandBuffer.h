//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_COMMANDBUFFER_H
#define EXPLOSION_COMMANDBUFFER_H

#include <functional>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/GpuRes.h>

namespace Explosion {
    class Driver;
    class Device;
    class CommandEncoder;
    class Signal;
    class GpuBuffer;

    using EncodingFunc = std::function<void(CommandEncoder* commandEncoder)>;

    class CommandBuffer : public GpuRes {
    public:
        explicit CommandBuffer(Driver& driver);
        ~CommandBuffer() override;
        const VkCommandBuffer& GetVkCommandBuffer();
        void EncodeCommands(const EncodingFunc& encodingFunc);
        void SubmitNow();
        void Submit(Signal* waitSignal, Signal* notifySignal);

    protected:
        void OnCreate() override;
        void OnDestroy() override;
        virtual void SetupSubmitInfo(VkSubmitInfo& submitInfo);

        Device& device;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

    private:
        void AllocateCommandBuffer();
        void FreeCommandBuffer();
    };

    class CommandEncoder {
    public:
        CommandEncoder(Driver& driver, CommandBuffer* commandBuffer);
        ~CommandEncoder();
        void CopyBuffer(GpuBuffer* srcBuffer, GpuBuffer* dstBuffer);

    private:
        Driver& driver;
        Device& device;
        CommandBuffer* commandBuffer = nullptr;
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
