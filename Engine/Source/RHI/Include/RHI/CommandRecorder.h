//
// Created by johnk on 21/2/2022.
//

#pragma once

#include <cstdint>
#include <optional>

#include <Common/Utility.h>
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
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
        Common::UVec3 origin;
        TextureAspect aspect;

        TextureSubResourceInfo();
        TextureSubResourceInfo& SetMipLevel(uint8_t inMipLevel);
        TextureSubResourceInfo& SetArray(uint8_t inBaseArrayLevel, uint8_t inArrayLayerNum);
        TextureSubResourceInfo& SetOrigin(const Common::UVec3& inOrigin);
        TextureSubResourceInfo& SetAspect(TextureAspect inAspect);
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

    struct ColorAttachment : public ColorAttachmentBase<ColorAttachment> {
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

    struct DepthStencilAttachment : public DepthStencilAttachmentBase<DepthStencilAttachment> {
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
        virtual void ResourceBarrier(const Barrier& barrier) = 0;
    };

    class CopyPassCommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(CopyPassCommandRecorder)
        virtual ~CopyPassCommandRecorder();

        virtual void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) = 0;
        virtual void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) = 0;
        virtual void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) = 0;
        virtual void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Common::UVec3& size) = 0;
        virtual void EndPass() = 0;
        virtual void Destroy() = 0;

    protected:
        CopyPassCommandRecorder();
    };

    class ComputePassCommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(ComputePassCommandRecorder)
        virtual ~ComputePassCommandRecorder();

        virtual void SetPipeline(ComputePipeline* pipeline) = 0;
        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) = 0;
        virtual void EndPass() = 0;
        virtual void Destroy() = 0;

    protected:
        ComputePassCommandRecorder();
    };

    class RasterPassCommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(RasterPassCommandRecorder)
        virtual ~RasterPassCommandRecorder();

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
        virtual void Destroy() = 0;

    protected:
        RasterPassCommandRecorder();
    };

    class CommandRecorder : public CommandCommandRecorder {
    public:
        NonCopyable(CommandRecorder)
        virtual ~CommandRecorder();

        virtual CopyPassCommandRecorder* BeginCopyPass() = 0;
        virtual ComputePassCommandRecorder* BeginComputePass() = 0;
        virtual RasterPassCommandRecorder* BeginRasterPass(const RasterPassBeginInfo& beginInfo) = 0;
        virtual void End() = 0;
        virtual void Destroy() = 0;

    protected:
        CommandRecorder();
    };
}

namespace RHI {
    template <typename Derived>
    ColorAttachmentBase<Derived>::ColorAttachmentBase(
        LoadOp inLoadOp, StoreOp inStoreOp, const Common::LinearColor& inClearValue)
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
    Derived& ColorAttachmentBase<Derived>::SetLoadOp(enum LoadOp inLoadOp)
    {
        loadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& ColorAttachmentBase<Derived>::SetStoreOp(enum StoreOp inStoreOp)
    {
        storeOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    DepthStencilAttachmentBase<Derived>::DepthStencilAttachmentBase(
        bool inDepthReadOnly,
        LoadOp inDepthLoadOp,
        StoreOp inDepthStoreOp,
        float inDepthClearValue,
        bool inStencilReadOnly,
        LoadOp inStencilLoadOp,
        StoreOp inStencilStoreOp,
        uint32_t inStencilClearValue)
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
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthClearValue(float inDepthClearValue)
    {
        depthClearValue = inDepthClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthLoadOp(LoadOp inLoadOp)
    {
        depthLoadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthStoreOp(StoreOp inStoreOp)
    {
        depthStoreOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetDepthReadOnly(bool inDepthReadOnly)
    {
        depthReadOnly = inDepthReadOnly;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilClearValue(uint32_t inStencilClearValue)
    {
        stencilClearValue = inStencilClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilLoadOp(LoadOp inLoadOp)
    {
        stencilLoadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilStoreOp(StoreOp inStoreOp)
    {
        stencilStoreOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& DepthStencilAttachmentBase<Derived>::SetStencilReadOnly(bool inStencilReadOnly)
    {
        stencilReadOnly = inStencilReadOnly;
        return *static_cast<Derived*>(this);
    }
}
