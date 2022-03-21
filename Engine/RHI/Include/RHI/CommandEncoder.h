//
// Created by johnk on 21/2/2022.
//

#ifndef EXPLOSION_RHI_COMMAND_ENCODER_H
#define EXPLOSION_RHI_COMMAND_ENCODER_H

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class Buffer;
    class Texture;
    class ComputePipeline;
    class GraphicsPipeline;
    class TextureView;
    class BindGroup;

    struct TextureSubResourceInfo {
        uint8_t mipLevels = 0;
        Extent<3> origin {};
        TextureAspect aspect = TextureAspect::ALL;
    };

    struct ComputePassBeginInfo {
        // TODO #see https://gpuweb.github.io/gpuweb/#typedefdef-gpucomputepasstimestampwrites
    };

    struct GraphicsPassColorAttachment {
        TextureView* view;
        TextureView* resolveTarget;
        Color<4> clearValue;
        LoadOp loadOp;
        LoadOp storeOp;
    };

    struct GraphicsPassDepthStencilAttachment {
        TextureView* view;
        float depthClearValue;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        bool depthReadOnly;
        uint32_t stencilClearValue;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        bool stencilReadOnly;
    };

    struct GraphicsPassBeginInfo {
        uint32_t colorAttachmentNum;
        const GraphicsPassColorAttachment* colorAttachments;
        const GraphicsPassDepthStencilAttachment* depthStencilAttachment;
        // TODO occlusionQuerySet #see https://gpuweb.github.io/gpuweb/#render-pass-encoder-creation
        // TODO timestampWrites #see https://gpuweb.github.io/gpuweb/#render-pass-encoder-creation
    };

    class ComputePassCommandEncoder {
    public:
        NON_COPYABLE(ComputePassCommandEncoder)
        virtual ~ComputePassCommandEncoder();

        virtual void SetPipeline(ComputePipeline* pipeline) = 0;
        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) = 0;
        virtual void DispatchIndirect(Buffer* indirectBuffer, size_t indirectOffset) = 0;
        virtual void EndPass() = 0;

    protected:
        ComputePassCommandEncoder();
    };

    class GraphicsPassCommandEncoder {
    public:
        NON_COPYABLE(GraphicsPassCommandEncoder)
        virtual ~GraphicsPassCommandEncoder();

        virtual void SetPipeline(GraphicsPipeline* pipeline) = 0;
        virtual void SetIndexBuffer(Buffer* buffer, const IndexFormat& indexFormat, size_t offset, size_t size) = 0;
        virtual void SetVertexBuffer(size_t slot, Buffer* buffer, size_t offset, size_t size) = 0;
        virtual void Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance) = 0;
        virtual void DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance) = 0;
        virtual void DrawIndirect(Buffer* indirectBuffer, size_t indirectOffset) = 0;
        virtual void DrawIndexedIndirect(Buffer* indirectBuffer, size_t indirectOffset) = 0;
        // TODO MultiIndirectDraw(...)
        virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void SetScissor(const Extent<2>& origin, const Extent<2>& extent) = 0;
        virtual void SetBlendConstant(const Color<4>& color) = 0;
        virtual void SetStencilReference(uint32_t reference) = 0;
        // TODO BeginOcclusionQuery(...)
        // TODO EndOcclusionQuery(...)
        // TODO ExecuteBundles(...)
        virtual void EndPass() = 0;

    protected:
        GraphicsPassCommandEncoder();
    };

    class CommandEncoder {
    public:
        NON_COPYABLE(CommandEncoder)
        virtual ~CommandEncoder();

        virtual void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) = 0;
        virtual void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) = 0;
        virtual void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) = 0;
        virtual void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size) = 0;
        virtual void ClearBuffer(Buffer* buffer, size_t offset, size_t size) = 0;
        // TODO WriteTimeStamp(...), #see https://gpuweb.github.io/gpuweb/#dom-gpucommandencoder-writetimestamp
        // TODO ResolveQuerySet(...), #see https://gpuweb.github.io/gpuweb/#dom-gpucommandencoder-resolvequeryset

        virtual ComputePassCommandEncoder* BeginComputePass(const ComputePassBeginInfo* beginInfo) = 0;
        virtual GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo) = 0;
        virtual void End() = 0;

    protected:
        CommandEncoder();
    };
}

#endif//EXPLOSION_RHI_COMMAND_ENCODER_H
