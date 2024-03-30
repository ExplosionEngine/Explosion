//
// Created by johnk on 2022/3/23.
//

#pragma once

#include <RHI/CommandEncoder.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12CommandBuffer;
    class DX12ComputePipeline;
    class DX12GraphicsPipeline;
    class DX12PipelineLayout;

    class DX12CommandEncoder : public CommandEncoder {
    public:
        NonCopyable(DX12CommandEncoder)
        explicit DX12CommandEncoder(DX12Device& inDevice, DX12CommandBuffer& inCmdBuffer);
        ~DX12CommandEncoder() override;

        void ResourceBarrier(const Barrier& inBarrier) override;
        CopyPassCommandEncoder* BeginCopyPass() override;
        ComputePassCommandEncoder* BeginComputePass() override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo& inBeginInfo) override;
        void End() override;
        void Destroy() override;

    private:
        DX12Device& device;
        DX12CommandBuffer& commandBuffer;
    };

    class DX12CopyPassCommandEncoder : public CopyPassCommandEncoder {
    public:
        NonCopyable(DX12CopyPassCommandEncoder)
        explicit DX12CopyPassCommandEncoder(DX12Device& inDevice, DX12CommandEncoder& inCmdEncoder, DX12CommandBuffer& inCmdBuffer);
        ~DX12CopyPassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // CopyPassCommandEncoder
        void CopyBufferToBuffer(Buffer* inSrcBuffer, size_t inSrcOffset, Buffer* inDestBuffer, size_t inDestOffset, size_t inSize) override;
        void CopyBufferToTexture(Buffer* inSrcBuffer, Texture* inDestTexture, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize) override;
        void CopyTextureToBuffer(Texture* inSrcTexture, Buffer* inDestBuffer, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize) override;
        void CopyTextureToTexture(Texture* inSrcTexture, const TextureSubResourceInfo* inSrcSubResourceInfo, Texture* inDestTexture, const TextureSubResourceInfo* inDestSubResourceInfo, const Common::UVec3& inSize) override;
        void EndPass() override;
        void Destroy() override;

    private:
        DX12Device& device;
        DX12CommandEncoder& commandEncoder;
        DX12CommandBuffer& commandBuffer;
    };

    class DX12ComputePassCommandEncoder : public ComputePassCommandEncoder {
    public:
        NonCopyable(DX12ComputePassCommandEncoder)
        explicit DX12ComputePassCommandEncoder(DX12Device& inDevice, DX12CommandEncoder& inCmdEncoder, DX12CommandBuffer& inCmdBuffer);
        ~DX12ComputePassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // ComputePassCommandEncoder
        void SetPipeline(ComputePipeline* inPipeline) override;
        void SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup) override;
        void Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ) override;
        void EndPass() override;
        void Destroy() override;

    private:
        DX12Device& device;
        DX12CommandEncoder& commandEncoder;
        DX12ComputePipeline* computePipeline;
        DX12CommandBuffer& commandBuffer;
    };

    class DX12GraphicsPassCommandEncoder : public GraphicsPassCommandEncoder {
    public:
        NonCopyable(DX12GraphicsPassCommandEncoder)
        explicit DX12GraphicsPassCommandEncoder(DX12Device& inDevice, DX12CommandEncoder& inCmdEncoder, DX12CommandBuffer& inCmdBuffer, const GraphicsPassBeginInfo& inBeginInfo);
        ~DX12GraphicsPassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // GraphicsPassCommandEncoder
        void SetPipeline(GraphicsPipeline* inPipeline) override;
        void SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup) override;
        void SetIndexBuffer(BufferView* inBufferView) override;
        void SetVertexBuffer(size_t inSlot, BufferView* inBufferView) override;
        void Draw(size_t inVertexCount, size_t inInstanceCount, size_t inFirstVertex, size_t inFirstInstance) override;
        void DrawIndexed(size_t inIndexCount, size_t inInstanceCount, size_t inFirstIndex, size_t inBaseVertex, size_t inFirstInstance) override;
        void SetViewport(float inX, float inY, float inWidth, float inHeight, float inMinDepth, float inMaxDepth) override;
        void SetScissor(uint32_t inLeft, uint32_t inTop, uint32_t inRight, uint32_t inBottom) override;
        void SetPrimitiveTopology(PrimitiveTopology inPrimitiveTopology) override;
        void SetBlendConstant(const float* inConstants) override;
        void SetStencilReference(uint32_t inReference) override;
        void EndPass() override;
        void Destroy() override;

    private:
        DX12Device& device;
        DX12CommandEncoder& commandEncoder;
        DX12GraphicsPipeline* graphicsPipeline;
        DX12CommandBuffer& commandBuffer;
    };
}
