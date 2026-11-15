#pragma once

#include <string>

#include <Render/SwapChainSurface.h>
#include <Runtime/Viewport.h>
#include <Runtime/Window.h>

struct GLFWwindow;

namespace Launch {
    class GameClient;

    struct GameWindowDesc {
        std::string title;
        uint32_t width;
        uint32_t height;
    };

    class GameWindow final : public Runtime::Window {
    public:
        GameWindow(RHI::Device& inDevice, Render::RenderThread& inRenderThread, GameClient& inClient, const GameWindowDesc& inDesc);
        ~GameWindow() override;

        bool ShouldClose() const override;
        void RequestClose() const override;
        void Tick(float inDeltaSeconds) override;
        void Render(float inDeltaSeconds) override;
        void WaitIdle() const override;

    private:
        GLFWwindow* window;
        Render::RenderThread& renderThread;
        Common::UniquePtr<Render::SwapChainSurface> swapChainSurface;
        Common::UniquePtr<Runtime::Viewport> viewport;
    };
}
