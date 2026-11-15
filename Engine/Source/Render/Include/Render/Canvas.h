//
// Created by johnk on 2026/7/8.
//

#pragma once

#include <string>

#include <Common/Memory.h>
#include <RHI/RHI.h>
#include <Render/RenderSurface.h>

namespace Render {
    struct CanvasDesc {
        CanvasDesc();

        uint32_t width;
        uint32_t height;
        RHI::PixelFormat format;
        std::string debugName;
    };

    class Canvas final : public RenderSurface {
    public:
        Canvas(RHI::Device& inDevice, const CanvasDesc& inDesc);
        ~Canvas() override;

        SurfaceTarget GetRenderTarget() const override;
        Common::UVec2 GetSize() const override;
        RHI::PixelFormat GetFormat() const override;
        RHI::TextureView* GetTextureView() const;
        void Resize(uint32_t inWidth, uint32_t inHeight) override;

    private:
        void CreateTarget();

        RHI::Device& device;
        uint32_t width;
        uint32_t height;
        RHI::PixelFormat format;
        std::string debugName;
        Common::UniquePtr<RHI::Texture> texture;
        Common::UniquePtr<RHI::TextureView> textureView;
        Common::UniquePtr<RHI::TextureView> renderTargetView;
    };
}
