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

    class VulkanCommandRecorder final : public CommandRecorder {
    public:
        NonCopyable(VulkanCommandRecorder)
        explicit VulkanCommandRecorder(VulkanDevice& inDevice, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanCommandRecorder() override;

        void ResourceBarrier(const Barrier& inBarrier) override;
        Common::UniqueRef<CopyPassCommandRecorder> BeginCopyPass() override;
        Common::UniqueRef<ComputePassCommandRecorder> BeginComputePass() override;
        Common::UniqueRef<RasterPassCommandRecorder> BeginRasterPass(const RasterPassBeginInfo& inBeginInfo) override;
        void End() override;

    private:
        VulkanDevice& device;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanCopyPassCommandRecorder final : public CopyPassCommandRecorder {
    public:
        NonCopyable(VulkanCopyPassCommandRecorder)
        explicit VulkanCopyPassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer);
        ~VulkanCopyPassCommandRecorder() override;

        // CommandCommandRecorder
        void ResourceBarrier(const Barrier& inBarrier) override;

        // CopyPassCommandRecorder
        void CopyBufferToBuffer(Buffer* src, Buffer* dst, const BufferCopyInfo& copyInfo) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const BufferTextureCopyInfo& copyInfo) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const BufferTextureCopyInfo& copyInfo) override;
        void CopyTextureToTexture(Texture* src, Texture* dst, const TextureCopyInfo& copyInfo) override;
        void EndPass() override;

    private:
        VulkanDevice& device;
        VulkanCommandRecorder& commandRecorder;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanComputePassCommandRecorder final : public ComputePassCommandRecorder {
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

    private:
        VulkanDevice& device;
        VulkanCommandRecorder& commandRecorder;
        VulkanCommandBuffer& commandBuffer;
    };

    class VulkanRasterPassCommandRecorder final : public RasterPassCommandRecorder {
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

    private:
        VulkanDevice& device;
        VulkanCommandRecorder& commandRecorder;
        VulkanCommandBuffer& commandBuffer;
        VkCommandBuffer nativeCmdBuffer;
        VulkanRasterPipeline* rasterPipeline;
    };

}
