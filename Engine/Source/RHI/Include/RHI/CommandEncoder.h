//
// Created by johnk on 21/2/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class Buffer;
    class BufferView;
    class Texture;
    class ComputePipeline;
    class GraphicsPipeline;
    class TextureView;
    class BindGroup;
    class SwapChain;
    struct Barrier;

    struct TextureSubResourceInfo {
        uint8_t mipLevel;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
        Extent<3> origin {};
        TextureAspect aspect = TextureAspect::COLOR;
    };

    struct GraphicsPassColorAttachment {
        TextureView* view;
        TextureView* resolve;
        ColorNormalized<4> clearValue;
        LoadOp loadOp;
        StoreOp storeOp;
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

    struct ComputePassBeginInfo {
        ComputePipeline* pipeline;
    };

    struct GraphicsPassBeginInfo {
        GraphicsPipeline* pipeline;
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

        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) = 0;
        virtual void EndPass() = 0;
        virtual void Destroy() = 0;

    protected:
        ComputePassCommandEncoder();
    };

    class GraphicsPassCommandEncoder {
    public:
        NON_COPYABLE(GraphicsPassCommandEncoder)
        virtual ~GraphicsPassCommandEncoder();

        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void SetIndexBuffer(BufferView* bufferView) = 0;
        virtual void SetVertexBuffer(size_t slot, BufferView* bufferView) = 0;
        virtual void Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance) = 0;
        virtual void DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance) = 0;
        virtual void SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) = 0;
        virtual void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) = 0;
        virtual void SetBlendConstant(const float*/*[4]*/ constants) = 0;
        virtual void SetStencilReference(uint32_t reference) = 0;
        // TODO DrawIndirect(...)
        // TODO DrawIndexedIndirect(...)
        // TODO MultiIndirectDraw(...)
        // TODO BeginOcclusionQuery(...)
        // TODO EndOcclusionQuery(...)
        // TODO ExecuteBundles(...)
        virtual void EndPass() = 0;
        virtual void Destroy() = 0;

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
        virtual void ResourceBarrier(const Barrier& barrier) = 0;
        // TODO WriteTimeStamp(...), #see https://gpuweb.github.io/gpuweb/#dom-gpucommandencoder-writetimestamp
        // TODO ResolveQuerySet(...), #see https://gpuweb.github.io/gpuweb/#dom-gpucommandencoder-resolvequeryset

        virtual ComputePassCommandEncoder* BeginComputePass(const ComputePassBeginInfo* beginInfo) = 0;
        virtual GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo) = 0;
        virtual void SwapChainSync(SwapChain* swapChain) = 0;
        virtual void End() = 0;
        virtual void Destroy() = 0;

    protected:
        CommandEncoder();
    };
}
