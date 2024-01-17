//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/CommandEncoder.h>

namespace RHI::Dummy {
    class DummyCommandBuffer;

    class DummyCommandEncoder : public CommandEncoder {
    public:
        NonCopyable(DummyCommandEncoder)
        explicit DummyCommandEncoder(const DummyCommandBuffer& inDummyCommandBuffer);
        ~DummyCommandEncoder() override;

        void ResourceBarrier(const Barrier& barrier) override;
        CopyPassCommandEncoder* BeginCopyPass() override;
        ComputePassCommandEncoder* BeginComputePass() override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo) override;
        void SwapChainSync(SwapChain *swapChain) override;
        void End() override;
        void Destroy() override;

    private:
        const DummyCommandBuffer& dummyCommandBuffer;
    };

    class DummyCopyPassCommandEncoder : public CopyPassCommandEncoder {
    public:
        NonCopyable(DummyCopyPassCommandEncoder)
        explicit DummyCopyPassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyCopyPassCommandEncoder();

        // CommandCommandEncoder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // CopyPassCommandEncoder
        void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) override;
        void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Common::UVec3& size) override;
        void EndPass() override;
        void Destroy() override;
    };

    class DummyComputePassCommandEncoder : public ComputePassCommandEncoder {
    public:
        NonCopyable(DummyComputePassCommandEncoder)
        explicit DummyComputePassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyComputePassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // ComputePassCommandEncoder
        void SetPipeline(ComputePipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup *bindGroup) override;
        void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) override;
        void EndPass() override;
        void Destroy() override;
    };
    
    class DummyGraphicsPassCommandEncoder : public GraphicsPassCommandEncoder {
    public:
        NonCopyable(DummyGraphicsPassCommandEncoder)
        explicit DummyGraphicsPassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyGraphicsPassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // GraphicsPassCommandEncoder
        void SetPipeline(GraphicsPipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) override;
        void SetIndexBuffer(BufferView* bufferView) override;
        void SetVertexBuffer(size_t slot, BufferView* bufferView) override;
        void Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance) override;
        void DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance) override;
        void SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) override;
        void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;
        void SetBlendConstant(const float*/*[4]*/ constants) override;
        void SetStencilReference(uint32_t reference) override;
        void EndPass() override;
        void Destroy() override;
    };
}
