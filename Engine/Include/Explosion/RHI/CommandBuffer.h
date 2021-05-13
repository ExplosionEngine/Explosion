//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_COMMANDBUFFER_H
#define EXPLOSION_COMMANDBUFFER_H

#include <functional>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/GpuRes.h>
#include <Explosion/RHI/Pipeline.h>

namespace Explosion {
    class Driver;
    class Device;
    class CommandEncoder;
    class Signal;
    class GpuBuffer;
    class RenderPass;
    class FrameBuffer;
    class Pipeline;

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
        struct RenderArea {
            int32_t x;
            int32_t y;
            uint32_t width;
            uint32_t height;
        };

        struct ClearValue {
            float r;
            float g;
            float b;
            float a;
        };

        struct RenderPassBeginInfo {
            FrameBuffer* frameBuffer;
            RenderArea renderArea;
            ClearValue clearValue;
        };

        CommandEncoder(Driver& driver, CommandBuffer* commandBuffer);
        ~CommandEncoder();
        void CopyBuffer(GpuBuffer* srcBuffer, GpuBuffer* dstBuffer);
        void BeginRenderPass(RenderPass* renderPass, const RenderPassBeginInfo& renderPassBeginInfo);
        void EndRenderPass();
        void BindGraphicsPipeline(Pipeline* pipeline);
        void BindVertexBuffer(uint32_t binding, GpuBuffer* vertexBuffer);
        void BindIndexBuffer(GpuBuffer* indexBuffer);
        void Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount);
        void DrawIndexed(uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset, uint32_t firstInstance, uint32_t instanceCount);
        void SetViewPort(const GraphicsPipeline::Viewport& viewport);
        void SetScissor(const GraphicsPipeline::Scissor& scissor);

    private:
        Driver& driver;
        Device& device;
        CommandBuffer* commandBuffer = nullptr;
    };

    class FrameOutputCommandBuffer : public CommandBuffer {
    public:
        explicit FrameOutputCommandBuffer(Driver& driver);
        ~FrameOutputCommandBuffer() override;

    protected:
        void SetupSubmitInfo(VkSubmitInfo& submitInfo) override;

    private:
        VkPipelineStageFlags waitStages = 0;
    };
}

#endif //EXPLOSION_COMMANDBUFFER_H
