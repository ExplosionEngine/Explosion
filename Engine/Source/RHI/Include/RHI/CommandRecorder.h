//
// Created by johnk on 21/2/2022.
//

#pragma once

#include <cstdint>
#include <optional>

#include <Common/Utility.h>
#include <Common/Math/Rect.h>
#include <Common/Math/Color.h>
#include <RHI/Common.h>

namespace RHI {
    class Buffer;
    class BufferView;
    class Texture;
    class ComputePipeline;
    class RasterPipeline;
    class TextureView;
    class BindGroup;
    class SwapChain;
    struct Barrier;

    struct TextureSubResourceInfo {
        uint8_t mipLevel;
        uint8_t arrayLayer;
        TextureAspect aspect;

        explicit TextureSubResourceInfo(uint8_t inMipLevel = 0, uint8_t inArrayLayer = 0, TextureAspect inAspect = TextureAspect::color);
        TextureSubResourceInfo& SetMipLevel(uint8_t inMipLevel);
        TextureSubResourceInfo& SetArrayLayer(uint8_t inArrayLayer);
        TextureSubResourceInfo& SetAspect(TextureAspect inAspect);
    };

    struct TextureSubResourceCopyFootprint {
        Common::UVec3 extent;
        size_t bytesPerPixel;
        size_t rowPitch;
        size_t slicePitch;
        size_t totalBytes;
    };

    struct BufferCopyInfo {
        size_t srcOffset;
        size_t dstOffset;
        size_t copySize;

        explicit BufferCopyInfo(size_t inSrcOffset = 0, size_t inDstOffset = 0, size_t inCopySize = 0);
        BufferCopyInfo& SetSrcOffset(size_t inSrcOffset);
        BufferCopyInfo& SetDstOffset(size_t inDstOffset);
        BufferCopyInfo& SetCopySize(size_t inCopySize);
    };

    struct TextureCopyInfo {
        TextureSubResourceInfo srcSubResource;
        Common::UVec3 srcOrigin;
        TextureSubResourceInfo dstSubResource;
        Common::UVec3 dstOrigin;
        Common::UVec3 copyRegion;

        explicit TextureCopyInfo(
            const TextureSubResourceInfo& inSrcSubResource = TextureSubResourceInfo(),
            const Common::UVec3& inSrcOrigin = Common::UVec3Consts::zero,
            const TextureSubResourceInfo& inDstSubResource = TextureSubResourceInfo(),
            const Common::UVec3& inDstOrigin = Common::UVec3Consts::zero,
            const Common::UVec3& inCopyRegion = Common::UVec3Consts::zero);

        TextureCopyInfo& SetSrcSubResource(const TextureSubResourceInfo& inSrcSubResource);
        TextureCopyInfo& SetSrcOrigin(const Common::UVec3& inSrcOrigin);
        TextureCopyInfo& SetDstSubResource(const TextureSubResourceInfo& inDstSubResource);
        TextureCopyInfo& SetDstOrigin(const Common::UVec3& inDstOrigin);
        TextureCopyInfo& SetCopyRegion(const Common::UVec3& inCopyRegion);
    };

    struct BufferTextureCopyInfo {
        size_t bufferOffset;
        TextureSubResourceInfo textureSubResource;
        Common::UVec3 textureOrigin;
        Common::UVec3 copyRegion;

        explicit BufferTextureCopyInfo(
            size_t inBufferOffset = 0,
            const TextureSubResourceInfo& inTextureSubResource = TextureSubResourceInfo(),
            const Common::UVec3& inTextureOrigin = Common::UVec3Consts::zero,
            const Common::UVec3& inCopyRegion = Common::UVec3Consts::zero);

        BufferTextureCopyInfo& SetBufferOffset(size_t inBufferOffset);
        BufferTextureCopyInfo& SetTextureSubResource(const TextureSubResourceInfo& inTextureSubResource);
        BufferTextureCopyInfo& SetTextureOrigin(const Common::UVec3& inTextureOrigin);
        BufferTextureCopyInfo& SetCopyRegion(const Common::UVec3& inCopyRegion);
    };

    template <typename Derived>
    struct ColorAttachmentBase {
        LoadOp loadOp;
        StoreOp storeOp;
        Common::LinearColor clearValue;

        explicit ColorAttachmentBase(
            LoadOp inLoadOp = LoadOp::load,
            StoreOp inStoreOp = StoreOp::discard,
            const Common::LinearColor& inClearValue = Common::LinearColorConsts::black);

        Derived& SetClearValue(const Common::LinearColor& inClearValue);
        Derived& SetLoadOp(LoadOp inLoadOp);
        Derived& SetStoreOp(StoreOp inStoreOp);
    };

    template <typename Derived>
    struct DepthStencilAttachmentBase {
        bool depthReadOnly;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        float depthClearValue;
        bool stencilReadOnly;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        uint32_t stencilClearValue;

        explicit DepthStencilAttachmentBase(
            bool inDepthReadOnly = true,
            LoadOp inDepthLoadOp = LoadOp::load,
            StoreOp inDepthStoreOp = StoreOp::discard,
            float inDepthClearValue = 0.0f,
            bool inStencilReadOnly = true,
            LoadOp inStencilLoadOp = LoadOp::load,
            StoreOp inStencilStoreOp = StoreOp::discard,
            uint32_t inStencilClearValue = 0);

        Derived& SetDepthClearValue(float inDepthClearValue);
        Derived& SetDepthLoadOp(LoadOp inLoadOp);
        Derived& SetDepthStoreOp(StoreOp inStoreOp);
        Derived& SetDepthReadOnly(bool inDepthReadOnly);
        Derived& SetStencilClearValue(uint32_t inStencilClearValue);
        Derived& SetStencilLoadOp(LoadOp inLoadOp);
        Derived& SetStencilStoreOp(StoreOp inStoreOp);
        Derived& SetStencilReadOnly(bool inStencilReadOnly);
    };

    struct ColorAttachment : ColorAttachmentBase<ColorAttachment> {
        TextureView* view;
        TextureView* resolveView;

        explicit ColorAttachment(
            TextureView* inView = nullptr,
            LoadOp inLoadOp = LoadOp::load,
            StoreOp inStoreOp = StoreOp::discard,
            const Common::LinearColor& inClearValue = Common::LinearColorConsts::black,
            TextureView* inResolveView = nullptr);

        ColorAttachment& SetView(TextureView* inView);
        ColorAttachment& SetResolveView(TextureView* inResolveView);
    };

    struct DepthStencilAttachment : DepthStencilAttachmentBase<DepthStencilAttachment> {
        TextureView* view;

        explicit DepthStencilAttachment(
            TextureView* inView = nullptr,
            bool inDepthReadOnly = true,
            LoadOp inDepthLoadOp = LoadOp::load,
            StoreOp inDepthStoreOp = StoreOp::discard,
            float inDepthClearValue = 0.0f,
            bool inStencilReadOnly = true,
            LoadOp inStencilLoadOp = LoadOp::load,
            StoreOp inStencilStoreOp = StoreOp::discard,
            uint32_t inStencilClearValue = 0);

        DepthStencilAttachment& SetView(TextureView* inView);
    };

    struct RasterPassBeginInfo {
        std::optional<DepthStencilAttachment> depthStencilAttachment;
        std::vector<ColorAttachment> colorAttachments;

        RasterPassBeginInfo();
        RasterPassBeginInfo& SetDepthStencilAttachment(const DepthStencilAttachment& inDepthStencilAttachment);
        RasterPassBeginInfo& AddColorAttachment(const ColorAttachment& inColorAttachment);
    };

    class CommandCommandRecorder {
    public:
        virtual ~CommandCommandRecorder();
        virtual void ResourceBarrier(const Barrier& barrier) = 0;
    };

    class CopyPassCommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(CopyPassCommandRecorder)
        ~CopyPassCommandRecorder() override;

        virtual void CopyBufferToBuffer(Buffer* src, Buffer* dst, const BufferCopyInfo& copyInfo) = 0;
        // NOTICE: CopyBufferToTexture/CopyTextureToBuffer treat buffer contains copy region (sub-image) data from offset
        virtual void CopyBufferToTexture(Buffer* src, Texture* dst, const BufferTextureCopyInfo& copyInfo) = 0;
        virtual void CopyTextureToBuffer(Texture* src, Buffer* dst, const BufferTextureCopyInfo& copyInfo) = 0;
        virtual void CopyTextureToTexture(Texture* src, Texture* dst, const TextureCopyInfo& copyInfo) = 0;
        virtual void EndPass() = 0;

    protected:
        CopyPassCommandRecorder();
    };

    class ComputePassCommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(ComputePassCommandRecorder)
        ~ComputePassCommandRecorder() override;

        virtual void SetPipeline(ComputePipeline* pipeline) = 0;
        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) = 0;
        virtual void EndPass() = 0;

    protected:
        ComputePassCommandRecorder();
    };

    class RasterPassCommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(RasterPassCommandRecorder)
        ~RasterPassCommandRecorder() override;

        virtual void SetPipeline(RasterPipeline* pipeline) = 0;
        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void SetIndexBuffer(BufferView* bufferView) = 0;
        virtual void SetVertexBuffer(size_t slot, BufferView* bufferView) = 0;
        virtual void Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance) = 0;
        virtual void DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance) = 0;
        virtual void SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) = 0;
        virtual void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) = 0;
        virtual void SetBlendConstant(const float*/*[4]*/ constants) = 0;
        virtual void SetStencilReference(uint32_t reference) = 0;
        // TODO DrawIndirect(...)
        // TODO DrawIndexedIndirect(...)
        // TODO MultiIndirectDraw(...)
        virtual void EndPass() = 0;

    protected:
        RasterPassCommandRecorder();
    };

    class CommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(CommandRecorder)
        ~CommandRecorder() override;

        virtual Common::UniquePtr<CopyPassCommandRecorder> BeginCopyPass() = 0;
        virtual Common::UniquePtr<ComputePassCommandRecorder> BeginComputePass() = 0;
        virtual Common::UniquePtr<RasterPassCommandRecorder> BeginRasterPass(const RasterPassBeginInfo& beginInfo) = 0;
        virtual void End() = 0;

    protected:
        CommandRecorder();
    };
}

namespace RHI {
    template <typename Derived>
    ColorAttachmentBase<Derived>::ColorAttachmentBase(
        const LoadOp inLoadOp,
        const StoreOp inStoreOp,
        const Common::LinearColor& inClearValue)
        : loadOp(inLoadOp)
        , storeOp(inStoreOp)
        , clearValue(inClearValue)
    {
    }

    template <typename Derived>
    Derived& ColorAttachmentBase<Derived>::SetClearValue(const Common::LinearColor& inClearValue)
    {
        clearValue = inClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& ColorAttachmentBase<Derived>::SetLoadOp(const LoadOp inLoadOp)
    {
        loadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& ColorAttachmentBase<Derived>::SetStoreOp(const StoreOp inStoreOp)
    {
        storeOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    DepthStencilAttachmentBase<Derived>::DepthStencilAttachmentBase(
        const bool inDepthReadOnly,
        const LoadOp inDepthLoadOp,
        const StoreOp inDepthStoreOp,
        const float inDepthClearValue,
        const bool inStencilReadOnly,
        const LoadOp inStencilLoadOp,
        const StoreOp inStencilStoreOp,
        const uint32_t inStencilClearValue)
        : depthReadOnly(inDepthReadOnly)
        , depthLoadOp(inDepthLoadOp)
        , depthStoreOp(inDepthStoreOp)
        , depthClearValue(inDepthClearValue)
        , stencilReadOnly(inStencilReadOnly)
        , stencilLoadOp(inStencilLoadOp)
        , stencilStoreOp(inStencilStoreOp)
        , stencilClearValue(inStencilClearValue)
    {
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthClearValue(const float inDepthClearValue)
    {
        depthClearValue = inDepthClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthLoadOp(const LoadOp inLoadOp)
    {
        depthLoadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthStoreOp(const StoreOp inStoreOp)
    {
        depthStoreOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthReadOnly(const bool inDepthReadOnly)
    {
        depthReadOnly = inDepthReadOnly;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilClearValue(const uint32_t inStencilClearValue)
    {
        stencilClearValue = inStencilClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilLoadOp(const LoadOp inLoadOp)
    {
        stencilLoadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilStoreOp(const StoreOp inStoreOp)
    {
        stencilStoreOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilReadOnly(const bool inStencilReadOnly)
    {
        stencilReadOnly = inStencilReadOnly;
        return *static_cast<Derived*>(this);
    }
}
