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
    class GraphicsPipeline;
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
        TextureSubResourceInfo& MipLevel(uint8_t inMipLevel);
        TextureSubResourceInfo& Array(uint8_t inBaseArrayLevel, uint8_t inArrayLayerNum);
        TextureSubResourceInfo& Origin(const Common::UVec3& inOrigin);
        TextureSubResourceInfo& Aspect(TextureAspect inAspect);
    };

    template <typename Derived>
    struct GraphicsPassColorAttachmentBase {
        Common::LinearColor clearValue;
        LoadOp loadOp;
        StoreOp storeOp;

        GraphicsPassColorAttachmentBase();
        Derived& ClearValue(const Common::LinearColor& inClearValue);
        Derived& LoadOp(LoadOp inLoadOp);
        Derived& StoreOp(StoreOp inStoreOp);
    };

    template <typename Derived>
    struct GraphicsPassDepthStencilAttachmentBase {
        float depthClearValue;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        bool depthReadOnly;
        uint32_t stencilClearValue;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        bool stencilReadOnly;

        GraphicsPassDepthStencilAttachmentBase();
        Derived& DepthClearValue(float inDepthClearValue);
        Derived& DepthLoadOp(LoadOp inLoadOp);
        Derived& DepthStoreOp(StoreOp inStoreOp);
        Derived& DepthReadOnly(bool inDepthReadOnly);
        Derived& StencilClearValue(uint32_t inStencilClearValue);
        Derived& StencilLoadOp(LoadOp inLoadOp);
        Derived& StencilStoreOp(StoreOp inStoreOp);
        Derived& StencilReadOnly(bool inStencilReadOnly);
    };

    struct GraphicsPassColorAttachment : public GraphicsPassColorAttachmentBase<GraphicsPassColorAttachment> {
        TextureView* view;
        TextureView* resolveView;

        GraphicsPassColorAttachment();
        GraphicsPassColorAttachment& View(TextureView* inView);
        GraphicsPassColorAttachment& ResolveView(TextureView* inResolveView);
    };

    struct GraphicsPassDepthStencilAttachment : public GraphicsPassDepthStencilAttachmentBase<GraphicsPassDepthStencilAttachment> {
        TextureView* view;

        GraphicsPassDepthStencilAttachment();
        GraphicsPassDepthStencilAttachment& View(TextureView* inView);
    };

    struct GraphicsPassBeginInfo {
        std::optional<GraphicsPassDepthStencilAttachment> depthStencilAttachment;
        std::vector<GraphicsPassColorAttachment> colorAttachments;

        GraphicsPassBeginInfo();
        GraphicsPassBeginInfo& DepthStencilAttachment(const GraphicsPassDepthStencilAttachment& inDepthStencilAttachment);
        GraphicsPassBeginInfo& ColorAttachment(const GraphicsPassColorAttachment& inColorAttachment);
    };

    class CommandCommandEncoder {
    public:
        virtual void ResourceBarrier(const Barrier& barrier) = 0;
    };

    class CopyPassCommandEncoder : public CommandCommandEncoder {
    public:
        NonCopyable(CopyPassCommandEncoder)
        virtual ~CopyPassCommandEncoder();

        virtual void CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size) = 0;
        virtual void CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) = 0;
        virtual void CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size) = 0;
        virtual void CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Common::UVec3& size) = 0;
        virtual void EndPass() = 0;
        virtual void Destroy() = 0;

    protected:
        CopyPassCommandEncoder();
    };

    class ComputePassCommandEncoder : public CommandCommandEncoder {
    public:
        NonCopyable(ComputePassCommandEncoder)
        virtual ~ComputePassCommandEncoder();

        virtual void SetPipeline(ComputePipeline* pipeline) = 0;
        virtual void SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup) = 0;
        virtual void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ) = 0;
        virtual void EndPass() = 0;
        virtual void Destroy() = 0;

    protected:
        ComputePassCommandEncoder();
    };

    class GraphicsPassCommandEncoder : public CommandCommandEncoder {
    public:
        NonCopyable(GraphicsPassCommandEncoder)
        virtual ~GraphicsPassCommandEncoder();

        virtual void SetPipeline(GraphicsPipeline* pipeline) = 0;
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
        GraphicsPassCommandEncoder();
    };

    class CommandEncoder : public CommandCommandEncoder {
    public:
        NonCopyable(CommandEncoder)
        virtual ~CommandEncoder();

        virtual CopyPassCommandEncoder* BeginCopyPass() = 0;
        virtual ComputePassCommandEncoder* BeginComputePass() = 0;
        virtual GraphicsPassCommandEncoder* BeginGraphicsPass(const GraphicsPassBeginInfo& beginInfo) = 0;
        virtual void End() = 0;
        virtual void Destroy() = 0;

    protected:
        CommandEncoder();
    };
}

namespace RHI {
    template <typename Derived>
    GraphicsPassColorAttachmentBase<Derived>::GraphicsPassColorAttachmentBase()
        : clearValue(Common::ColorConsts::black.ToLinearColor())
        , loadOp(LoadOp::load)
        , storeOp(StoreOp::discard)
    {
    }

    template <typename Derived>
    Derived& GraphicsPassColorAttachmentBase<Derived>::ClearValue(const Common::LinearColor& inClearValue)
    {
        clearValue = inClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassColorAttachmentBase<Derived>::LoadOp(enum LoadOp inLoadOp)
    {
        loadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassColorAttachmentBase<Derived>::StoreOp(enum StoreOp inStoreOp)
    {
        storeOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    GraphicsPassDepthStencilAttachmentBase<Derived>::GraphicsPassDepthStencilAttachmentBase()
        : depthClearValue(1.0f)
        , depthLoadOp(LoadOp::load)
        , depthStoreOp(StoreOp::discard)
        , depthReadOnly(true)
        , stencilClearValue(0)
        , stencilLoadOp(LoadOp::load)
        , stencilStoreOp(StoreOp::discard)
        , stencilReadOnly(true)
    {
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::DepthClearValue(float inDepthClearValue)
    {
        depthClearValue = inDepthClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::DepthLoadOp(LoadOp inLoadOp)
    {
        depthLoadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::DepthStoreOp(StoreOp inStoreOp)
    {
        depthStoreOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::DepthReadOnly(bool inDepthReadOnly)
    {
        depthReadOnly = inDepthReadOnly;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::StencilClearValue(uint32_t inStencilClearValue)
    {
        stencilClearValue = inStencilClearValue;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::StencilLoadOp(LoadOp inLoadOp)
    {
        stencilLoadOp = inLoadOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::StencilStoreOp(StoreOp inStoreOp)
    {
        stencilStoreOp = inStoreOp;
        return *static_cast<Derived*>(this);
    }

    template <typename Derived>
    Derived& GraphicsPassDepthStencilAttachmentBase<Derived>::StencilReadOnly(bool inStencilReadOnly)
    {
        stencilReadOnly = inStencilReadOnly;
        return *static_cast<Derived*>(this);
    }
}
