//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/CommandEncoder.h>

namespace RHI::Dummy {
    class DummyCommandBuffer;

    class DummyComputePassCommandEncoder : public RHI::ComputePassCommandEncoder {
    public:
        NON_COPYABLE(DummyComputePassCommandEncoder)
        explicit DummyComputePassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyComputePassCommandEncoder() override;

        void SetPipeline(ComputePipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup *bindGroup) override;
        void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) override;
        void EndPass() override;
        void Destroy() override;
    };
    
    class DummyGraphicsPassCommandEncoder : public RHI::GraphicsPassCommandEncoder {
    public:
        NON_COPYABLE(DummyGraphicsPassCommandEncoder)
        explicit DummyGraphicsPassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyGraphicsPassCommandEncoder() override;

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
    
    class DummyCommandEncoder : public CommandEncoder {
    public:
        NON_COPYABLE(DummyCommandEncoder)
        explicit DummyCommandEncoder(const DummyCommandBuffer& inDummyCommandBuffer);
        ~DummyCommandEncoder() override;
        
        void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size) override;
        void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size) override;
        void ResourceBarrier(const Barrier& barrier) override;
        // TODO WriteTimeStamp(...), #see https://gpuweb.github.io/gpuweb/#dom-gpucommandencoder-writetimestamp
        // TODO ResolveQuerySet(...), #see https://gpuweb.github.io/gpuweb/#dom-gpucommandencoder-resolvequeryset
        ComputePassCommandEncoder* BeginComputePass() override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo) override;
        void SwapChainSync(SwapChain *swapChain) override;
        void End() override;
        void Destroy() override;

    private:
        const DummyCommandBuffer& dummyCommandBuffer;
    };
}
