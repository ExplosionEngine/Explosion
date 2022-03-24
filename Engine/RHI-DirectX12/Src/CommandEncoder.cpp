//
// Created by johnk on 2022/3/23.
//

#include <RHI/DirectX12/CommandEncoder.h>

namespace RHI::DirectX12 {
    DX12ComputePassCommandEncoder::DX12ComputePassCommandEncoder(DX12CommandBuffer& commandBuffer) : ComputePassCommandEncoder(), commandBuffer(commandBuffer)
    {
        // TODO
    }

    DX12ComputePassCommandEncoder::~DX12ComputePassCommandEncoder() = default;

    void DX12ComputePassCommandEncoder::SetPipeline(ComputePipeline* pipeline)
    {
        // TODO
    }

    void DX12ComputePassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
        // TODO
    }

    void DX12ComputePassCommandEncoder::Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ)
    {
        // TODO
    }

    void DX12ComputePassCommandEncoder::DispatchIndirect(Buffer* indirectBuffer, size_t indirectOffset)
    {
        // TODO
    }

    void DX12ComputePassCommandEncoder::EndPass()
    {
        // TODO
    }

    DX12GraphicsPassCommandEncoder::DX12GraphicsPassCommandEncoder(DX12CommandBuffer& commandBuffer) : GraphicsPassCommandEncoder(), commandBuffer(commandBuffer)
    {
        // TODO
    }

    DX12GraphicsPassCommandEncoder::~DX12GraphicsPassCommandEncoder() = default;

    void DX12GraphicsPassCommandEncoder::SetPipeline(GraphicsPipeline* pipeline)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetIndexBuffer(Buffer* buffer, const IndexFormat& indexFormat, size_t offset, size_t size)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, Buffer* buffer, size_t offset, size_t size)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::DrawIndirect(Buffer* indirectBuffer, size_t indirectOffset)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::DrawIndexedIndirect(Buffer* indirectBuffer, size_t indirectOffset)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetScissor(const Extent<2>& origin, const Extent<2>& extent)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetBlendConstant(const Color<4>& color)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {
        // TODO
    }

    void DX12GraphicsPassCommandEncoder::EndPass()
    {
        // TODO
    }

    DX12CommandEncoder::DX12CommandEncoder(DX12CommandBuffer& commandBuffer) : CommandEncoder(), commandBuffer(commandBuffer) {}

    DX12CommandEncoder::~DX12CommandEncoder() = default;

    void DX12CommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo , const Extent<3>& size)
    {
        // TODO
    }

    ComputePassCommandEncoder* DX12CommandEncoder::BeginComputePass(const ComputePassBeginInfo* beginInfo)
    {
        return new DX12ComputePassCommandEncoder(commandBuffer);
    }

    GraphicsPassCommandEncoder* DX12CommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new DX12GraphicsPassCommandEncoder(commandBuffer);
    }

    void DX12CommandEncoder::End()
    {
        // TODO
    }
}
