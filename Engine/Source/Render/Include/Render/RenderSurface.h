//
// Created by johnk on 2026/7/8.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>
#include <Common/Math/Vector.h>
#include <RHI/Texture.h>
#include <RHI/Synchronous.h>

namespace Render {
    struct SurfaceTarget {
        RHI::Texture* texture;
        RHI::TextureView* renderTargetView;
        RHI::TextureState beforeRenderState;
        RHI::TextureState afterRenderState;
        RHI::Semaphore* waitSemaphore = nullptr;
        RHI::Semaphore* signalSemaphore = nullptr;
    };

    class RenderSurface {
    public:
        virtual ~RenderSurface();

        NonCopyable(RenderSurface)
        NonMovable(RenderSurface)

        // Read on the render thread, after acquiring the backbuffer when this is a window surface.
        virtual SurfaceTarget GetRenderTarget() const = 0;
        virtual Common::UVec2 GetSize() const = 0;
        virtual RHI::PixelFormat GetFormat() const = 0;
        virtual void Resize(uint32_t inWidth, uint32_t inHeight) = 0;

    protected:
        RenderSurface();
    };
}
