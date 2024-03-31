//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/CommandEncoder.h>

namespace RHI::Vulkan {
    class VulkanGpu;
    class VulkanDevice;
    class VulkanCommandBuffer;
    class VulkanGraphicsPipeline;

    class VulkanCommandEncoder : public CommandEncoder {
    public:
        NonCopyable(VulkanCommandEncoder)
        explicit VulkanCommandEncoder(VulkanDevice& inDevice, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanCommandEncoder() override;

        void ResourceBarrier(const Barrier& inBarrier) override;
        CopyPassCommandEncoder* BeginCopyPass() override;
        ComputePassCommandEncoder* BeginComputePass() override;
        GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo& inBeginInfo) override;
        void End() override;
        void Destroy() override;

    private:
        VulkanDevice& device;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanCopyPassCommandEncoder : public CopyPassCommandEncoder {
    public:
        NonCopyable(VulkanCopyPassCommandEncoder)
        explicit VulkanCopyPassCommandEncoder(VulkanDevice& inDevice, VulkanCommandEncoder& inCmdEncoder, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanCopyPassCommandEncoder() override;

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
        VulkanDevice& device;
        VulkanCommandEncoder& commandEncoder;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanComputePassCommandEncoder : public ComputePassCommandEncoder {
    public:
        NonCopyable(VulkanComputePassCommandEncoder)
        explicit VulkanComputePassCommandEncoder(VulkanDevice& inDevice, VulkanCommandEncoder& inCmdEncoder, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanComputePassCommandEncoder() override;

        // CommandCommandEncoder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // ComputePassCommandEncoder
        void SetPipeline(ComputePipeline* inPipeline) override;
        void SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup) override;
        void Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ) override;
        void EndPass() override;
        void Destroy() override;

    private:
        VulkanDevice& device;
        VulkanCommandEncoder& commandEncoder;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanGraphicsPassCommandEncoder : public GraphicsPassCommandEncoder {
    public:
        NonCopyable(VulkanGraphicsPassCommandEncoder)
        explicit VulkanGraphicsPassCommandEncoder(VulkanDevice& inDevice, VulkanCommandEncoder& inCmdEncoder, VulkanCommandBuffer& inCmdBuffer, const GraphicsPassBeginInfo& inBeginInfo);
        ~VulkanGraphicsPassCommandEncoder() override;

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
        VulkanDevice& device;
        VulkanCommandEncoder& commandEncoder;
        VulkanCommandBuffer& commandBuffer;
        VkCommandBuffer nativeCmdBuffer;
        VulkanGraphicsPipeline* graphicsPipeline;
    };

}
