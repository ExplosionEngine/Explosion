//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/CommandEncoder.h>

namespace RHI::Dummy {
    DummyComputePassCommandEncoder::DummyComputePassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer)
    {
    }

    DummyComputePassCommandEncoder::~DummyComputePassCommandEncoder() = default;

    void DummyComputePassCommandEncoder::SetPipeline(ComputePipeline* pipeline)
    {
    }

    void DummyComputePassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
    }

    void DummyComputePassCommandEncoder::Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ)
    {
    }

    void DummyComputePassCommandEncoder::EndPass()
    {
    }

    void DummyComputePassCommandEncoder::Destroy()
    {
        delete this;
    }

    DummyGraphicsPassCommandEncoder::DummyGraphicsPassCommandEncoder(const DummyCommandBuffer& dummyCommandBuffer)
    {
    }

    DummyGraphicsPassCommandEncoder::~DummyGraphicsPassCommandEncoder() = default;

    void DummyGraphicsPassCommandEncoder::SetPipeline(GraphicsPipeline* pipeline)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetIndexBuffer(BufferView* bufferView)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView* bufferView)
    {
    }

    void DummyGraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {
    }

    void DummyGraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetBlendConstant(const float* constants)
    {
    }

    void DummyGraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {
    }

    void DummyGraphicsPassCommandEncoder::EndPass()
    {
    }

    void DummyGraphicsPassCommandEncoder::Destroy()
    {
        delete this;
    }

    DummyCommandEncoder::DummyCommandEncoder(const DummyCommandBuffer& inDummyCommandBuffer)
        : dummyCommandBuffer(inDummyCommandBuffer)
    {
    }

    DummyCommandEncoder::~DummyCommandEncoder() = default;

    void DummyCommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {
    }

    void DummyCommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
    }

    void DummyCommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
    }

    void DummyCommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size)
    {
    }

    void DummyCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
    }

    ComputePassCommandEncoder* DummyCommandEncoder::BeginComputePass()
    {
        return new DummyComputePassCommandEncoder(dummyCommandBuffer);
    }

    GraphicsPassCommandEncoder* DummyCommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new DummyGraphicsPassCommandEncoder(dummyCommandBuffer);
    }

    void DummyCommandEncoder::SwapChainSync(SwapChain* swapChain)
    {
    }

    void DummyCommandEncoder::End()
    {
    }

    void DummyCommandEncoder::Destroy()
    {
        delete this;
    }
}
