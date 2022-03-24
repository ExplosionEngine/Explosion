//
// Created by johnk on 2022/3/23.
//

#pragma once

#include <RHI/CommandEncoder.h>

namespace RHI::DirectX12 {
    class DX12CommandBuffer;

    class DX12CommandEncoder : public CommandEncoder {
    public:
        NON_COPYABLE(DX12CommandEncoder)
        explicit DX12CommandEncoder(DX12CommandBuffer& commandBuffer);
        ~DX12CommandEncoder() override;

        void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) override;
        void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size) override;

        ComputePassCommandEncoder* BeginComputePass(const ComputePassBeginInfo* beginInfo) override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo) override;
        void End() override;

    private:
        DX12CommandBuffer& commandBuffer;
    };

    class DX12ComputePassCommandEncoder : public ComputePassCommandEncoder {
    public:
        NON_COPYABLE(DX12ComputePassCommandEncoder)
        explicit DX12ComputePassCommandEncoder(DX12CommandBuffer& commandBuffer);
        ~DX12ComputePassCommandEncoder() override;

        void SetPipeline(ComputePipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) override;
        void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) override;
        void DispatchIndirect(Buffer* indirectBuffer, size_t indirectOffset) override;
        void EndPass() override;

    private:
        DX12CommandBuffer& commandBuffer;
    };

    class DX12GraphicsPassCommandEncoder : public GraphicsPassCommandEncoder {
    public:
        NON_COPYABLE(DX12GraphicsPassCommandEncoder)
        explicit DX12GraphicsPassCommandEncoder(DX12CommandBuffer& commandBuffer);
        ~DX12GraphicsPassCommandEncoder() override;

        void SetPipeline(GraphicsPipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) override;
        void SetIndexBuffer(Buffer* buffer, const IndexFormat& indexFormat, size_t offset, size_t size) override;
        void SetVertexBuffer(size_t slot, Buffer* buffer, size_t offset, size_t size) override;
        void Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance) override;
        void DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance) override;
        void DrawIndirect(Buffer* indirectBuffer, size_t indirectOffset) override;
        void DrawIndexedIndirect(Buffer* indirectBuffer, size_t indirectOffset) override;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(const Extent<2> &origin, const Extent<2> &extent) override;
        void SetBlendConstant(const Color<4>& color) override;
        void SetStencilReference(uint32_t reference) override;
        void EndPass() override;

    private:
        DX12CommandBuffer& commandBuffer;
    };
}