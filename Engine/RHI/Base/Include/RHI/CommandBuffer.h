//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_COMMANDBUFFER_H
#define EXPLOSION_COMMANDBUFFER_H

#include <functional>

#include <RHI/Enum.h>
#include <RHI/GraphicsPipeline.h>

namespace Explosion::RHI {
    class CommandEncoder;
    class FrameBuffer;
    class Signal;
    class Buffer;
    class RenderPass;
    class DescriptorSet;

    using EncodingFunc = std::function<void(CommandEncoder* commandEncoder)>;

    class CommandBuffer {
    public:
        virtual ~CommandBuffer();
        virtual void EncodeCommands(const EncodingFunc& encodingFunc) = 0;
        virtual void SubmitNow() = 0;
        virtual void Submit(Signal* waitSignal, Signal* notifySignal, PipelineStageFlags waitStages) = 0;

    protected:
        CommandBuffer();
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

        virtual ~CommandEncoder();
        virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer) = 0;
        virtual void BeginRenderPass(RenderPass* renderPass, const RenderPassBeginInfo& renderPassBeginInfo) = 0;
        virtual void EndRenderPass() = 0;
        virtual void BindGraphicsPipeline(GraphicsPipeline* pipeline) = 0;
        virtual void BindVertexBuffer(uint32_t binding, Buffer* vertexBuffer) = 0;
        virtual void BindIndexBuffer(Buffer* indexBuffer) = 0;
        virtual void Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount) = 0;
        virtual void DrawIndexed(uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset, uint32_t firstInstance, uint32_t instanceCount) = 0;
        virtual void SetViewPort(const GraphicsPipeline::Viewport& viewport) = 0;
        virtual void SetScissor(const GraphicsPipeline::Scissor& scissor) = 0;
        virtual void BindDescriptorSet(const std::vector<DescriptorSet*>& descriptorSets) = 0;

    protected:
        explicit CommandEncoder();
    };
}

#endif //EXPLOSION_COMMANDBUFFER_H
