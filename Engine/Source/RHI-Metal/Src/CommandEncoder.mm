//
// Created by Zach Lee on 2022/10/28.
//

#include <Metal/CommandEncoder.h>
#include <Metal/Pipeline.h>
#include <Metal/BufferView.h>
#include <Metal/TextureView.h>
#include <Metal/Common.h>
#include <Metal/SwapChain.h>

namespace RHI::Metal {
    MTLCommandEncoder::MTLCommandEncoder(MTLDevice& dev, id<MTLCommandBuffer> cmd) : device(dev), commandBuffer(cmd)
    {
    }

    MTLCommandEncoder::~MTLCommandEncoder()
    {
    }

    void MTLCommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {

    }

    void MTLCommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {

    }

    void MTLCommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {

    }

    void MTLCommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size)
    {

    }

    void MTLCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {

    }

    ComputePassCommandEncoder* MTLCommandEncoder::BeginComputePass(const ComputePassBeginInfo* beginInfo)
    {

    }

    GraphicsPassCommandEncoder* MTLCommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new MTLGraphicsPassCommandEncoder(device, commandBuffer, beginInfo);
    }

    void MTLCommandEncoder::End()
    {
    }

    void MTLCommandEncoder::Destroy()
    {
        delete this;
    }

    void MTLCommandEncoder::SwapChainSync(SwapChain* swapChain)
    {
        static_cast<MTLSwapChain*>(swapChain)->AddSignalEventToCmd(commandBuffer);
    }

    // Compute Pass Encoder
    MTLComputePassCommandEncoder::MTLComputePassCommandEncoder(MTLDevice &dev, id<MTLCommandBuffer> cmd) : device(dev), commandBuffer(cmd)
    {
    }

    MTLComputePassCommandEncoder::~MTLComputePassCommandEncoder()
    {
    }

    void MTLComputePassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
    }

    void MTLComputePassCommandEncoder::Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ)
    {
    }

    void MTLComputePassCommandEncoder::EndPass()
    {
    }

    void MTLComputePassCommandEncoder::Destroy()
    {
        delete this;
    }

    // Graphics Pass Encoder
    MTLGraphicsPassCommandEncoder::MTLGraphicsPassCommandEncoder(MTLDevice &dev, id<MTLCommandBuffer> cmd, const GraphicsPassBeginInfo* beginInfo)
        : device(dev), commandBuffer(cmd)
    {
        MTLRenderPassDescriptor *descriptor = [[MTLRenderPassDescriptor alloc] init];
        for (uint32_t i = 0; i < beginInfo->colorAttachmentNum; ++i) {
            const GraphicsPassColorAttachment &colorInfo = beginInfo->colorAttachments[i];
            descriptor.colorAttachments[i].texture = static_cast<MTLTextureView*>(colorInfo.view)->GetTexture();
            descriptor.colorAttachments[i].clearColor = MakeClearColor(colorInfo.clearValue);
            descriptor.colorAttachments[i].loadAction = MTLEnumCast<LoadOp, MTLLoadAction>(colorInfo.loadOp);
            descriptor.colorAttachments[i].storeAction = MTLEnumCast<StoreOp, MTLStoreAction>(colorInfo.storeOp);
        }

        renderEncoder = [cmd renderCommandEncoderWithDescriptor: descriptor];

        MTLGraphicsPipeline *mtlPipeline = static_cast<MTLGraphicsPipeline*>(beginInfo->pipeline);
        id<MTLRenderPipelineState> pipeline = mtlPipeline->GetNativePipeline();
        [renderEncoder setRenderPipelineState: pipeline];
        [descriptor release];
    }

    MTLGraphicsPassCommandEncoder::~MTLGraphicsPassCommandEncoder()
    {
    }

    void MTLGraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {

    }

    void MTLGraphicsPassCommandEncoder::SetIndexBuffer(BufferView *bufferView)
    {
        MTLBufferView *mtiBufferView = static_cast<MTLBufferView*>(bufferView);
        indexBuffer = mtiBufferView->GetNativeBuffer();
        indexType = mtiBufferView->GetIndexType();
        indexOffset = mtiBufferView->GetOffset();
    }

    void MTLGraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView *bufferView)
    {
        MTLBufferView *mtlBufferView = static_cast<MTLBufferView*>(bufferView);

        [renderEncoder setVertexBuffer: mtlBufferView->GetNativeBuffer()
                                offset: mtlBufferView->GetOffset()
                               atIndex: slot];
    }

    void MTLGraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {
        [renderEncoder drawPrimitives: primitiveType
                          vertexStart: firstVertex
                          vertexCount: vertexCount
                        instanceCount: instanceCount
                         baseInstance: firstInstance];
    }

    void MTLGraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {
        [renderEncoder drawIndexedPrimitives: primitiveType
                                  indexCount: indexCount
                                   indexType: MTLIndexTypeUInt32
                                 indexBuffer: indexBuffer
                           indexBufferOffset: indexOffset
                               instanceCount: instanceCount
                                  baseVertex: baseVertex
                                baseInstance: firstInstance];
    }

    void MTLGraphicsPassCommandEncoder::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        [renderEncoder setViewport: MTLViewport{x, y, width, height, minDepth, maxDepth}];
    }

    void MTLGraphicsPassCommandEncoder::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
        [renderEncoder setScissorRect: MTLScissorRect{left, top, right - left, bottom - top}];
    }

    void MTLGraphicsPassCommandEncoder::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
    {
    }

    void MTLGraphicsPassCommandEncoder::SetBlendConstant(const float *constants)
    {

    }

    void MTLGraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {

    }

    void MTLGraphicsPassCommandEncoder::EndPass()
    {
        [renderEncoder endEncoding];
    }

    void MTLGraphicsPassCommandEncoder::Destroy()
    {
        delete this;
    }
}
