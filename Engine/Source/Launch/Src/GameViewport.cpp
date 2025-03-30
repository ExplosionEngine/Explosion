//
// Created by johnk on 2025/2/19.
//

#include <Launch/GameViewport.h>
#include <Runtime/Engine.h>

namespace Launch::Internal {
    static void* GetGlfwPlatformWindow(GLFWwindow* inWindow)
    {
#if PLATFORM_WINDOWS
        return glfwGetWin32Window(inWindow);
#elif PLATFORM_MACOS
        return glfwGetCocoaView(inWindow);
#else
        Unimplement();
        return nullptr;
#endif
    }
}

namespace Launch {
    GameViewport::GameViewport(const GameViewportDesc& inDesc)
        : renderModule(Runtime::EngineHolder::Get().GetRenderModule())
        , client(*this)
    {
        glfwInit();
        window = glfwCreateWindow(static_cast<int>(inDesc.width), static_cast<int>(inDesc.height), inDesc.title.c_str(), nullptr, nullptr);

        device = renderModule.GetDevice();
        imageReadySemaphore = device->CreateSemaphore();
        renderFinishedSemaphore = device->CreateSemaphore();

        surface = device->CreateSurface(RHI::SurfaceCreateInfo(Internal::GetGlfwPlatformWindow(window)));
        RecreateSwapChain(inDesc.width, inDesc.height);
    }

    GameViewport::~GameViewport()
    {
        WaitDeviceIdle();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    Runtime::Client& GameViewport::GetClient()
    {
        return client;
    }

    Runtime::PresentInfo GameViewport::GetNextPresentInfo()
    {
        const auto backTextureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());

        Runtime::PresentInfo result;
        result.backTexture = swapChain->GetTexture(backTextureIndex);
        result.imageReadySemaphore = imageReadySemaphore.Get();
        result.renderFinishedSemaphore = renderFinishedSemaphore.Get();
        return result;
    }

    uint32_t GameViewport::GetWidth() const
    {
        int width;
        glfwGetWindowSize(window, &width, nullptr);
        return static_cast<uint32_t>(width);
    }

    uint32_t GameViewport::GetHeight() const
    {
        int height;
        glfwGetWindowSize(window, nullptr, &height);
        return static_cast<uint32_t>(height);
    }

    void GameViewport::Resize(uint32_t inWidth, uint32_t inHeight)
    {
        glfwSetWindowSize(window, static_cast<int>(inWidth), static_cast<int>(inHeight));
        RecreateSwapChain(inWidth, inHeight);
    }

    bool GameViewport::ShouldClose() const
    {
        return static_cast<bool>(glfwWindowShouldClose(window));
    }

    void GameViewport::PollEvents() const // NOLINT
    {
        glfwPollEvents();
    }

    void GameViewport::RecreateSwapChain(uint32_t inWidth, uint32_t inHeight)
    {
        static std::vector<RHI::PixelFormat> formatQualifiers = {
            RHI::PixelFormat::rgba8Unorm,
            RHI::PixelFormat::bgra8Unorm
        };

        WaitDeviceIdle();
        if (swapChain.Valid()) {
            swapChain.Reset();
        }

        std::optional<RHI::PixelFormat> pixelFormat = {};
        for (const auto format : formatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                pixelFormat = format;
                break;
            }
        }
        Assert(pixelFormat.has_value());

        swapChain = device->CreateSwapChain(
            RHI::SwapChainCreateInfo()
                .SetPresentQueue(device->GetQueue(RHI::QueueType::graphics, 0))
                .SetSurface(surface.Get())
                .SetTextureNum(2)
                .SetFormat(pixelFormat.value())
                .SetWidth(inWidth)
                .SetHeight(inHeight)
                .SetPresentMode(RHI::PresentMode::immediately));
    }

    void GameViewport::WaitDeviceIdle() const
    {
        renderModule.GetRenderThread().Flush();

        const Common::UniquePtr<RHI::Fence> fence = device->CreateFence(false);
        device->GetQueue(RHI::QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();
    }
}
