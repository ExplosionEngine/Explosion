//
// Created by johnk on 2025/2/19.
//

#pragma once

#include <GLFW/glfw3.h>
#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include <RHI/RHI.h>
#include <Runtime/Viewport.h>
#include <Launch/GameClient.h>
#include <Render/RenderModule.h>

namespace Launch {
    struct GameViewportDesc {
        std::string title;
        uint32_t width;
        uint32_t height;
    };

    class GameViewport final : public Runtime::Viewport {
    public:
        explicit GameViewport(const GameViewportDesc& inDesc);
        ~GameViewport() override;

        Runtime::Client& GetClient() override;
        Runtime::PresentInfo GetNextPresentInfo() override;
        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;
        void Resize(uint32_t inWidth, uint32_t inHeight) override;

        bool ShouldClose() const;
        void PollEvents() const;
        void RecreateSwapChain(uint32_t inWidth, uint32_t inHeight);
        void WaitDeviceIdle() const;

    private:
        Render::RenderModule& renderModule;
        GameClient client;
        GLFWwindow* window;
        RHI::Device* device;
        Common::UniquePtr<RHI::Semaphore> imageReadySemaphore;
        Common::UniquePtr<RHI::Semaphore> renderFinishedSemaphore;
        Common::UniquePtr<RHI::Surface> surface;
        Common::UniquePtr<RHI::SwapChain> swapChain;
    };
}
