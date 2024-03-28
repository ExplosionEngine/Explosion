//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vulkan/vulkan.h>
#include <RHI/CommandEncoder.h>

namespace RHI::Vulkan {

    class VKGpu;
    class VKDevice;
    class VKCommandBuffer;
    class VKGraphicsPipeline;

    class VKCommandEncoder : public CommandEncoder {
    public:
        NonCopyable(VKCommandEncoder)
        explicit VKCommandEncoder(VKDevice& device, VKCommandBuffer& commandBuffer);
        ~VKCommandEncoder() override;

        void ResourceBarrier(const Barrier& barrier) override;
        CopyPassCommandEncoder* BeginCopyPass() override;
        ComputePassCommandEncoder* BeginComputePass() override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo& beginInfo) override;
        void End() override;
        void Destroy() override;

    private:
        VKDevice& device;
        VKCommandBuffer& commandBuffer;
    };

    class VKCopyPassCommandEncoder : public CopyPassCommandEncoder {
    public:
        NonCopyable(VKCopyPassCommandEncoder)
        explicit VKCopyPassCommandEncoder(VKDevice& device, VKCommandEncoder& commandEncoder, VKCommandBuffer& commandBuffer);
        ~VKCopyPassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // CopyPassCommandEncoder
        void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) override;
        void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Common::UVec3& size) override;
        void EndPass() override;
        void Destroy() override;

    private:
        VKDevice& device;
        VKCommandEncoder& commandEncoder;
        VKCommandBuffer& commandBuffer;
    };

    class VKComputePassCommandEncoder : public ComputePassCommandEncoder {
    public:
        NonCopyable(VKComputePassCommandEncoder)
        explicit VKComputePassCommandEncoder(VKDevice& device, VKCommandEncoder& commandEncoder, VKCommandBuffer& commandBuffer);
        ~VKComputePassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // ComputePassCommandEncoder
        void SetPipeline(ComputePipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) override;
        void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) override;
        void EndPass() override;
        void Destroy() override;

    private:
        VKDevice& device;
        VKCommandEncoder& commandEncoder;
        VKCommandBuffer& commandBuffer;
    };

    class VKGraphicsPassCommandEncoder : public GraphicsPassCommandEncoder {
    public:
        NonCopyable(VKGraphicsPassCommandEncoder)
        explicit VKGraphicsPassCommandEncoder(VKDevice& device, VKCommandEncoder& commandEncoder, VKCommandBuffer& commandBuffer, const GraphicsPassBeginInfo& beginInfo);
        ~VKGraphicsPassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // GraphicsPassCommandEncoder
        void SetPipeline(GraphicsPipeline* pipeline) override;
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
        VKDevice& device;
        VKCommandEncoder& commandEncoder;
        VKCommandBuffer& commandBuffer;
        VkCommandBuffer cmdHandle;
        VKGraphicsPipeline* graphicsPipeline;
    };

}
