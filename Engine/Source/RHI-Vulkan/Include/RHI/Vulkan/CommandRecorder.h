//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/CommandRecorder.h>

namespace RHI::Vulkan {
    class VulkanGpu;
    class VulkanDevice;
    class VulkanCommandBuffer;
    class VulkanRasterPipeline;
    class VulkanComputePipeline;

    class VulkanCommandRecorder : public CommandRecorder {
    public:
        NonCopyable(VulkanCommandRecorder)
        explicit VulkanCommandRecorder(VulkanDevice& inDevice, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanCommandRecorder() override;

        void ResourceBarrier(const Barrier& inBarrier) override;
        CopyPassCommandRecorder* BeginCopyPass() override;
        ComputePassCommandRecorder* BeginComputePass() override;
        RasterPassCommandRecorder* BeginRasterPass(const RasterPassBeginInfo& inBeginInfo) override;
        void End() override;
        void Destroy() override;

    private:
        VulkanDevice& device;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanCopyPassCommandRecorder : public CopyPassCommandRecorder {
    public:
        NonCopyable(VulkanCopyPassCommandRecorder)
        explicit VulkanCopyPassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanCopyPassCommandRecorder() override;

        // CommandCommandRecorder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // CopyPassCommandRecorder
        void CopyBufferToBuffer(Buffer* inSrcBuffer, size_t inSrcOffset, Buffer* inDestBuffer, size_t inDestOffset, size_t inSize) override;
        void CopyBufferToTexture(Buffer* inSrcBuffer, Texture* inDestTexture, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize) override;
        void CopyTextureToBuffer(Texture* inSrcTexture, Buffer* inDestBuffer, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize) override;
        void CopyTextureToTexture(Texture* inSrcTexture, const TextureSubResourceInfo* inSrcSubResourceInfo, Texture* inDestTexture, const TextureSubResourceInfo* inDestSubResourceInfo, const Common::UVec3& inSize) override;
        void EndPass() override;
        void Destroy() override;

    private:
        VulkanDevice& device;
        VulkanCommandRecorder& commandRecorder;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanComputePassCommandRecorder : public ComputePassCommandRecorder {
    public:
        NonCopyable(VulkanComputePassCommandRecorder)
        explicit VulkanComputePassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanComputePassCommandRecorder() override;

        // CommandCommandRecorder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // ComputePassCommandRecorder
        void SetPipeline(ComputePipeline* inPipeline) override;
        void SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup) override;
        void Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ) override;
        void EndPass() override;
        void Destroy() override;

    private:
        VulkanDevice& device;
        VulkanCommandRecorder& commandRecorder;
        VulkanCommandBuffer& commandBuffer;
        VulkanComputePipeline* computePipeline = nullptr;
    };

    class VulkanRasterPassCommandRecorder : public RasterPassCommandRecorder {
    public:
        NonCopyable(VulkanRasterPassCommandRecorder)
        explicit VulkanRasterPassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer, const RasterPassBeginInfo& inBeginInfo);
        ~VulkanRasterPassCommandRecorder() override;

        // CommandCommandRecorder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // RasterPassCommandRecorder
        void SetPipeline(RasterPipeline* inPipeline) override;
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
        VulkanCommandRecorder& commandRecorder;
        VulkanCommandBuffer& commandBuffer;
        VkCommandBuffer nativeCmdBuffer;
        VulkanRasterPipeline* rasterPipeline = nullptr;
    };

}
