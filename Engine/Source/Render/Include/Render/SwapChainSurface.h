//
// Created by johnk on 2026/7/8.
//

#pragma once

#include <vector>

#include <Common/Memory.h>
#include <RHI/RHI.h>
#include <Render/RenderSurface.h>

namespace Render {
    class SwapChainSurface final : public RenderSurface {
    public:
        SwapChainSurface(RHI::Device& inDevice, void* inNativeWindow, uint32_t inWidth, uint32_t inHeight);
        ~SwapChainSurface() override;

        SurfaceTarget GetRenderTarget() const override;
        Common::UVec2 GetSize() const override;
        RHI::PixelFormat GetFormat() const override;
        void Resize(uint32_t inWidth, uint32_t inHeight) override;
        void AcquireBackTexture();
        void Present();
        RHI::Device& GetDevice() const;
        void WaitRenderingIdle() const;

    private:
        static constexpr uint8_t swapChainTextureNum = 2;

        void ReleaseSwapChainResources();
        void RecreateSwapChain(uint32_t inWidth, uint32_t inHeight);
        RHI::Semaphore* GetRenderFinishedSemaphore() const;

        RHI::Device& device;
        Common::UniquePtr<RHI::Surface> surface;
        Common::UniquePtr<RHI::SwapChain> swapChain;
        std::vector<RHI::Texture*> swapChainTextures;
        std::vector<Common::UniquePtr<RHI::TextureView>> swapChainTextureViews;
        std::vector<RHI::TextureState> swapChainTextureStates;
        Common::UniquePtr<RHI::Semaphore> imageReadySemaphore;
        std::vector<Common::UniquePtr<RHI::Semaphore>> renderFinishedSemaphores;
        RHI::PixelFormat swapChainFormat;
        uint32_t currentBackTextureIndex;
        uint32_t width;
        uint32_t height;
    };
}
