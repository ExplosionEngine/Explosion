//
// Created by johnk on 2026/7/8.
//

#include <algorithm>
#include <optional>
#include <vector>

#include <Render/SwapChainSurface.h>

#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

namespace Render::WindowPresentation::Internal {
    static uint32_t ClampExtent(uint32_t inValue)
    {
        return std::max(1u, inValue);
    }
}

namespace Render {
    SwapChainSurface::SwapChainSurface(RHI::Device& inDevice, void* inNativeWindow, uint32_t inWidth, uint32_t inHeight)
        : device(inDevice)
        , imageReadySemaphore((device.CreateSemaphore)())
        , swapChainFormat(RHI::PixelFormat::max)
        , currentBackTextureIndex(0)
        , width(0)
        , height(0)
    {
        surface = device.CreateSurface(RHI::SurfaceCreateInfo(inNativeWindow));
        RecreateSwapChain(WindowPresentation::Internal::ClampExtent(inWidth), WindowPresentation::Internal::ClampExtent(inHeight));
    }

    SwapChainSurface::~SwapChainSurface()
    {
        WaitRenderingIdle();
        ReleaseSwapChainResources();
    }

    SurfaceTarget SwapChainSurface::GetRenderTarget() const
    {
        Assert(currentBackTextureIndex < swapChainTextures.size());
        return {
            .texture = swapChainTextures[currentBackTextureIndex],
            .renderTargetView = swapChainTextureViews[currentBackTextureIndex].Get(),
            .beforeRenderState = swapChainTextureStates[currentBackTextureIndex],
            .afterRenderState = RHI::TextureState::present,
            .waitSemaphore = imageReadySemaphore.Get(),
            .signalSemaphore = GetRenderFinishedSemaphore()
        };
    }

    void SwapChainSurface::AcquireBackTexture()
    {
        currentBackTextureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());
        Assert(currentBackTextureIndex < swapChainTextures.size());
    }

    void SwapChainSurface::Resize(uint32_t inWidth, uint32_t inHeight)
    {
        const auto newWidth = WindowPresentation::Internal::ClampExtent(inWidth);
        const auto newHeight = WindowPresentation::Internal::ClampExtent(inHeight);
        if (width == newWidth && height == newHeight) {
            return;
        }
        WaitRenderingIdle();
        RecreateSwapChain(newWidth, newHeight);
    }

    void SwapChainSurface::Present()
    {
        swapChain->Present(GetRenderFinishedSemaphore());
        swapChainTextureStates[currentBackTextureIndex] = RHI::TextureState::present;
    }

    RHI::Device& SwapChainSurface::GetDevice() const
    {
        return device;
    }

    Common::UVec2 SwapChainSurface::GetSize() const
    {
        return { width, height };
    }

    RHI::PixelFormat SwapChainSurface::GetFormat() const
    {
        return swapChainFormat;
    }

    RHI::Semaphore* SwapChainSurface::GetRenderFinishedSemaphore() const
    {
        Assert(currentBackTextureIndex < renderFinishedSemaphores.size());
        return renderFinishedSemaphores[currentBackTextureIndex].Get();
    }

    void SwapChainSurface::WaitRenderingIdle() const
    {
        const auto fence = device.CreateFence(false);
        device.GetQueue(RHI::QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();
    }

    void SwapChainSurface::ReleaseSwapChainResources()
    {
        swapChainTextureViews.clear();
        swapChain.Reset();
        swapChainTextures.clear();
        swapChainTextureStates.clear();
        renderFinishedSemaphores.clear();
        currentBackTextureIndex = 0;
    }

    void SwapChainSurface::RecreateSwapChain(uint32_t inWidth, uint32_t inHeight)
    {
        static std::vector<RHI::PixelFormat> formatQualifiers = {
            RHI::PixelFormat::rgba8Unorm,
            RHI::PixelFormat::bgra8Unorm
        };

        ReleaseSwapChainResources();

        std::optional<RHI::PixelFormat> pixelFormat = {};
        for (const auto format : formatQualifiers) {
            if (device.CheckSwapChainFormatSupport(surface.Get(), format, RHI::ColorSpace::srgbNonLinear)) {
                pixelFormat = format;
                break;
            }
        }
        Assert(pixelFormat.has_value());
        swapChainFormat = pixelFormat.value();
        width = inWidth;
        height = inHeight;

        swapChain = device.CreateSwapChain(
            RHI::SwapChainCreateInfo()
                .SetPresentQueue(device.GetQueue(RHI::QueueType::graphics, 0))
                .SetSurface(surface.Get())
                .SetTextureNum(swapChainTextureNum)
                .SetFormat(swapChainFormat)
                .SetWidth(width)
                .SetHeight(height)
                .SetPresentMode(RHI::PresentMode::immediately));

        const auto actualTextureNum = swapChain->GetTextureNum();
        swapChainTextures.resize(actualTextureNum);
        swapChainTextureViews.resize(actualTextureNum);
        swapChainTextureStates.resize(actualTextureNum);
        renderFinishedSemaphores.resize(actualTextureNum);
        currentBackTextureIndex = 0;

        for (uint8_t i = 0; i < actualTextureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);
            swapChainTextureStates[i] = swapChainTextures[i]->GetCreateInfo().initialState;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(
                RHI::TextureViewCreateInfo()
                    .SetDimension(RHI::TextureViewDimension::tv2D)
                    .SetMipLevels(0, 1)
                    .SetArrayLayers(0, 1)
                    .SetAspect(RHI::TextureAspect::color)
                    .SetType(RHI::TextureViewType::colorAttachment));
            renderFinishedSemaphores[i] = (device.CreateSemaphore)();
        }
    }
}
