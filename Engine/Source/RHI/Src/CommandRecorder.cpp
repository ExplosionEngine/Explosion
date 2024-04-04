//
// Created by johnk on 21/2/2022.
//

#include <RHI/CommandRecorder.h>

namespace RHI {
    TextureSubResourceInfo::TextureSubResourceInfo()
        : mipLevel(0)
        , baseArrayLayer(0)
        , arrayLayerNum(0)
        , origin()
        , aspect(TextureAspect::color)
    {
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetMipLevel(uint8_t inMipLevel)
    {
        mipLevel = inMipLevel;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetArray(uint8_t inBaseArrayLevel, uint8_t inArrayLayerNum)
    {
        baseArrayLayer = inBaseArrayLevel;
        arrayLayerNum = inArrayLayerNum;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetOrigin(const Common::UVec3& inOrigin)
    {
        origin = inOrigin;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetAspect(TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    ColorAttachment::ColorAttachment(
        TextureView* inView, LoadOp inLoadOp, StoreOp inStoreOp, const Common::LinearColor& inClearValue, TextureView* inResolveView)
        : ColorAttachmentBase<ColorAttachment>(inLoadOp, inStoreOp, inClearValue)
        , view(inView)
        , resolveView(inResolveView)
    {
    }

    ColorAttachment& ColorAttachment::SetView(TextureView* inView)
    {
        view = inView;
        return *this;
    }

    ColorAttachment& ColorAttachment::SetResolveView(TextureView* inResolveView)
    {
        resolveView = inResolveView;
        return *this;
    }

    DepthStencilAttachment::DepthStencilAttachment(
        TextureView* inView,
        bool inDepthReadOnly,
        LoadOp inDepthLoadOp,
        StoreOp inDepthStoreOp,
        float inDepthClearValue,
        bool inStencilReadOnly,
        LoadOp inStencilLoadOp,
        StoreOp inStencilStoreOp,
        uint32_t inStencilClearValue)
        : DepthStencilAttachmentBase<DepthStencilAttachment>(
            inDepthReadOnly, inDepthLoadOp, inDepthStoreOp, inDepthClearValue,
            inStencilReadOnly, inStencilLoadOp, inStencilStoreOp, inStencilClearValue)
        , view(inView)
    {
    }

    DepthStencilAttachment& DepthStencilAttachment::SetView(TextureView* inView)
    {
        view = inView;
        return *this;
    }

    RasterPassBeginInfo::RasterPassBeginInfo() = default;

    RasterPassBeginInfo& RasterPassBeginInfo::SetDepthStencilAttachment(const DepthStencilAttachment& inDepthStencilAttachment)
    {
        depthStencilAttachment = inDepthStencilAttachment;
        return *this;
    }

    RasterPassBeginInfo& RasterPassBeginInfo::AddColorAttachment(const ColorAttachment& inColorAttachment)
    {
        colorAttachments.emplace_back(inColorAttachment);
        return *this;
    }

    CommandRecorder::CommandRecorder() = default;

    CommandRecorder::~CommandRecorder() = default;

    CopyPassCommandRecorder::CopyPassCommandRecorder() = default;

    CopyPassCommandRecorder::~CopyPassCommandRecorder() = default;

    ComputePassCommandRecorder::ComputePassCommandRecorder() = default;

    ComputePassCommandRecorder::~ComputePassCommandRecorder() = default;

    RasterPassCommandRecorder::RasterPassCommandRecorder() = default;

    RasterPassCommandRecorder::~RasterPassCommandRecorder() = default;
}
