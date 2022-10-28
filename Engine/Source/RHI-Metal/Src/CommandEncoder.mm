//
// Created by Zach Lee on 2022/10/28.
//

#include <Metal/CommandEncoder.h>

namespace RHI::Metal {
    MTLCommandEncoder::MTLCommandEncoder(MTLDevice& dev, MTLCommandBuffer& cmd) : device(dev), commandBuffer(cmd)
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
        new MTLGraphicsPassCommandEncoder(device, commandBuffer, beginInfo);
    }

    void MTLCommandEncoder::End()
    {
    }

    // Compute Pass Encoder
    MTLComputePassCommandEncoder::MTLComputePassCommandEncoder(MTLDevice &dev, MTLCommandBuffer &cmd) : device(dev), commandBuffer(cmd)
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

    // Graphics Pass Encoder
    MTLGraphicsPassCommandEncoder::MTLGraphicsPassCommandEncoder(MTLDevice &dev, MTLCommandBuffer &cmd, const GraphicsPassBeginInfo* beginInfo)
        : device(dev), commandBuffer(cmd)
    {
    }

    MTLGraphicsPassCommandEncoder::~MTLGraphicsPassCommandEncoder()
    {
    }

    void MTLGraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {

    }

    void MTLGraphicsPassCommandEncoder::SetIndexBuffer(BufferView *bufferView)
    {

    }

    void MTLGraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView *bufferView)
    {

    }

    void MTLGraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {

    }

    void MTLGraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {

    }

    void MTLGraphicsPassCommandEncoder::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {

    }

    void MTLGraphicsPassCommandEncoder::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {

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

    }
}
