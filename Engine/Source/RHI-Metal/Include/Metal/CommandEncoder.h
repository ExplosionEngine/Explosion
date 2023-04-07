//
// Created by Zach Lee on 2022/10/28.
//

#pragma once

#include <RHI/CommandEncoder.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLDevice;
    class MTLCommandBuffer;

    class MTLCommandEncoder : public CommandEncoder {
    public:
        NON_COPYABLE(MTLCommandEncoder)
        explicit MTLCommandEncoder(MTLDevice& dev, id<MTLCommandBuffer> cmd);
        ~MTLCommandEncoder() override;

        void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) override;
        void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size) override;
        void ResourceBarrier(const Barrier& barrier) override;
        void SwapChainSync(SwapChain* swapChain) override;
        ComputePassCommandEncoder* BeginComputePass(const ComputePassBeginInfo* beginInfo) override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo) override;
        void End() override;
        void Destroy() override;

    private:
        MTLDevice& device;
        id<MTLCommandBuffer> commandBuffer;
    };

    class MTLComputePassCommandEncoder : public ComputePassCommandEncoder {
    public:
        NON_COPYABLE(MTLComputePassCommandEncoder)
        explicit MTLComputePassCommandEncoder(MTLDevice &dev, id<MTLCommandBuffer> cmd);
        ~MTLComputePassCommandEncoder() override;

        void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) override;
        void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) override;
        void EndPass() override;
        void Destroy() override;

    private:
        MTLDevice& device;
        id<MTLCommandBuffer> commandBuffer = nil;
    };

    class MTLGraphicsPassCommandEncoder : public GraphicsPassCommandEncoder {
    public:
        NON_COPYABLE(MTLGraphicsPassCommandEncoder)
        explicit MTLGraphicsPassCommandEncoder(MTLDevice &dev, id<MTLCommandBuffer> cmd, const GraphicsPassBeginInfo* beginInfo);
        ~MTLGraphicsPassCommandEncoder() override;

        void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) override;
        void SetIndexBuffer(BufferView *bufferView) override;
        void SetVertexBuffer(size_t slot, BufferView *bufferView) override;
        void Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance) override;
        void DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance) override;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) override;
        void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;
        void SetBlendConstant(const float *constants) override;
        void SetStencilReference(uint32_t reference) override;
        void EndPass() override;
        void Destroy() override;

    private:
        void CommitRenderResources();

        MTLDevice& device;
        id<MTLCommandBuffer> commandBuffer = nil;
        id<MTLRenderCommandEncoder> renderEncoder = nil;

        id<MTLBuffer> indexBuffer = nil;
        uint32_t indexOffset = 0;
        MTLIndexType indexType = MTLIndexTypeUInt32;

        MTLPrimitiveType primitiveType = MTLPrimitiveTypeTriangle;
    };
}
