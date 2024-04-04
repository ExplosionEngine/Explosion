//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/CommandRecorder.h>

namespace RHI::Dummy {
    class DummyCommandBuffer;

    class DummyCommandRecorder : public CommandRecorder {
    public:
        NonCopyable(DummyCommandRecorder)
        explicit DummyCommandRecorder(const DummyCommandBuffer& inDummyCommandBuffer);
        ~DummyCommandRecorder() override;

        void ResourceBarrier(const Barrier& barrier) override;
        CopyPassCommandRecorder* BeginCopyPass() override;
        ComputePassCommandRecorder* BeginComputePass() override;
        RasterPassCommandRecorder* BeginRasterPass(const RasterPassBeginInfo& beginInfo) override;
        void End() override;
        void Destroy() override;

    private:
        const DummyCommandBuffer& dummyCommandBuffer;
    };

    class DummyCopyPassCommandRecorder : public CopyPassCommandRecorder {
    public:
        NonCopyable(DummyCopyPassCommandRecorder)
        explicit DummyCopyPassCommandRecorder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyCopyPassCommandRecorder();

        // CommandCommandRecorder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // CopyPassCommandRecorder
        void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) override;
        void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) override;
        void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) override;
        void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Common::UVec3& size) override;
        void EndPass() override;
        void Destroy() override;
    };

    class DummyComputePassCommandRecorder : public ComputePassCommandRecorder {
    public:
        NonCopyable(DummyComputePassCommandRecorder)
        explicit DummyComputePassCommandRecorder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyComputePassCommandRecorder() override;

        // CommandCommandRecorder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // ComputePassCommandRecorder
        void SetPipeline(ComputePipeline* pipeline) override;
        void SetBindGroup(uint8_t layoutIndex, BindGroup *bindGroup) override;
        void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) override;
        void EndPass() override;
        void Destroy() override;
    };
    
    class DummyRasterPassCommandRecorder : public RasterPassCommandRecorder {
    public:
        NonCopyable(DummyRasterPassCommandRecorder)
        explicit DummyRasterPassCommandRecorder(const DummyCommandBuffer& dummyCommandBuffer);
        ~DummyRasterPassCommandRecorder() override;

        // CommandCommandRecorder
        void ResourceBarrier(const RHI::Barrier& barrier) override;

        // RasterPassCommandRecorder
        void SetPipeline(RasterPipeline* pipeline) override;
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
