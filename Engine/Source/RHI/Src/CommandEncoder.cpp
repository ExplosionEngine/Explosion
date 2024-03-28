//
// Created by johnk on 21/2/2022.
//

#include <RHI/CommandEncoder.h>

namespace RHI {
    TextureSubResourceInfo::TextureSubResourceInfo()
        : mipLevel(0)
        , baseArrayLayer(0)
        , arrayLayerNum(0)
        , origin()
        , aspect(TextureAspect::color)
    {
    }

    TextureSubResourceInfo& TextureSubResourceInfo::MipLevel(uint8_t inMipLevel)
    {
        mipLevel = inMipLevel;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::Array(uint8_t inBaseArrayLevel, uint8_t inArrayLayerNum)
    {
        baseArrayLayer = inBaseArrayLevel;
        arrayLayerNum = inArrayLayerNum;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::Origin(const Common::UVec3& inOrigin)
    {
        origin = inOrigin;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::Aspect(TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    GraphicsPassColorAttachment::GraphicsPassColorAttachment()
        : GraphicsPassColorAttachmentBase<GraphicsPassColorAttachment>()
        , view(nullptr)
        , resolveView(nullptr)
    {
    }

    GraphicsPassColorAttachment& GraphicsPassColorAttachment::View(TextureView* inView)
    {
        view = inView;
        return *this;
    }

    GraphicsPassColorAttachment& GraphicsPassColorAttachment::ResolveView(TextureView* inResolveView)
    {
        resolveView = inResolveView;
        return *this;
    }

    GraphicsPassDepthStencilAttachment::GraphicsPassDepthStencilAttachment()
        : GraphicsPassDepthStencilAttachmentBase<GraphicsPassDepthStencilAttachment>()
        , view(nullptr)
    {
    }

    GraphicsPassDepthStencilAttachment& GraphicsPassDepthStencilAttachment::View(TextureView* inView)
    {
        view = inView;
        return *this;
    }

    GraphicsPassBeginInfo::GraphicsPassBeginInfo() = default;

    GraphicsPassBeginInfo& GraphicsPassBeginInfo::DepthStencilAttachment(const GraphicsPassDepthStencilAttachment& inDepthStencilAttachment)
    {
        depthStencilAttachment = inDepthStencilAttachment;
        return *this;
    }

    GraphicsPassBeginInfo& GraphicsPassBeginInfo::ColorAttachment(const GraphicsPassColorAttachment& inColorAttachment)
    {
        colorAttachments.emplace_back(inColorAttachment);
        return *this;
    }

    CommandEncoder::CommandEncoder() = default;

    CommandEncoder::~CommandEncoder() = default;

    CopyPassCommandEncoder::CopyPassCommandEncoder() = default;

    CopyPassCommandEncoder::~CopyPassCommandEncoder() = default;

    ComputePassCommandEncoder::ComputePassCommandEncoder() = default;

    ComputePassCommandEncoder::~ComputePassCommandEncoder() = default;

    GraphicsPassCommandEncoder::GraphicsPassCommandEncoder() = default;

    GraphicsPassCommandEncoder::~GraphicsPassCommandEncoder() = default;
}
