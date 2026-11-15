//
// Created by johnk on 2026/7/8.
//

#include <algorithm>

#include <Render/Canvas.h>

namespace Render::CanvasResources::Internal {
    static uint32_t ClampExtent(uint32_t inValue)
    {
        return std::max(1u, inValue);
    }
}

namespace Render {
    CanvasDesc::CanvasDesc()
        : width(1)
        , height(1)
        , format(RHI::PixelFormat::rgba8Unorm)
        , debugName("canvas")
    {
    }

    Canvas::Canvas(RHI::Device& inDevice, const CanvasDesc& inDesc)
        : device(inDevice)
        , width(CanvasResources::Internal::ClampExtent(inDesc.width))
        , height(CanvasResources::Internal::ClampExtent(inDesc.height))
        , format(inDesc.format)
        , debugName(inDesc.debugName)
    {
        CreateTarget();
    }

    Canvas::~Canvas() = default;

    SurfaceTarget Canvas::GetRenderTarget() const
    {
        return {
            .texture = texture.Get(),
            .renderTargetView = renderTargetView.Get(),
            .beforeRenderState = RHI::TextureState::shaderReadOnly,
            .afterRenderState = RHI::TextureState::shaderReadOnly
        };
    }

    RHI::TextureView* Canvas::GetTextureView() const
    {
        return textureView.Get();
    }

    Common::UVec2 Canvas::GetSize() const
    {
        return { width, height };
    }

    RHI::PixelFormat Canvas::GetFormat() const
    {
        return format;
    }

    void Canvas::Resize(uint32_t inWidth, uint32_t inHeight)
    {
        const uint32_t newWidth = CanvasResources::Internal::ClampExtent(inWidth);
        const uint32_t newHeight = CanvasResources::Internal::ClampExtent(inHeight);
        if (newWidth == width && newHeight == height) {
            return;
        }

        width = newWidth;
        height = newHeight;
        textureView.Reset();
        renderTargetView.Reset();
        texture.Reset();
        CreateTarget();
    }

    void Canvas::CreateTarget()
    {
        texture = device.CreateTexture(
            RHI::TextureCreateInfo()
                .SetType(RHI::TextureType::t2D)
                .SetWidth(width)
                .SetHeight(height)
                .SetDepthOrArraySize(1)
                .SetFormat(format)
                .SetUsages(RHI::TextureUsageBits::renderAttachment | RHI::TextureUsageBits::textureBinding)
                .SetMipLevels(1)
                .SetSamples(1)
                .SetInitialState(RHI::TextureState::shaderReadOnly)
                .SetDebugName(debugName));
        textureView = texture->CreateTextureView(
            RHI::TextureViewCreateInfo()
                .SetDimension(RHI::TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(RHI::TextureAspect::color)
                .SetType(RHI::TextureViewType::textureBinding));
        renderTargetView = texture->CreateTextureView(
            RHI::TextureViewCreateInfo()
                .SetDimension(RHI::TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(RHI::TextureAspect::color)
                .SetType(RHI::TextureViewType::colorAttachment));
    }
}
