#include <algorithm>

#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

#include <Launch/GameViewport.h>
#include <Launch/GameWindow.h>

namespace Launch::GameWindowing::Internal {
    uint32_t windowCount = 0;

    static GLFWwindow* CreateGlfwWindow(const GameWindowDesc& inDesc)
    {
        if (windowCount == 0) {
            const auto initialized = glfwInit();
            Assert(initialized == GLFW_TRUE);
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
        auto* window = glfwCreateWindow(static_cast<int>(inDesc.width), static_cast<int>(inDesc.height), inDesc.title.c_str(), nullptr, nullptr);
        Assert(window != nullptr);
        ++windowCount;
        return window;
    }

    static void* NativeHandle(GLFWwindow* inWindow)
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

    static Common::UVec2 FramebufferSize(GLFWwindow* inWindow)
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(inWindow, &width, &height);
        return { static_cast<uint32_t>(std::max(1, width)), static_cast<uint32_t>(std::max(1, height)) };
    }
}

namespace Launch {
    GameWindow::GameWindow(RHI::Device& inDevice, Render::RenderThread& inRenderThread, GameClient& inClient, const GameWindowDesc& inDesc)
        : window(GameWindowing::Internal::CreateGlfwWindow(inDesc))
        , renderThread(inRenderThread)
        , swapChainSurface(Common::MakeUnique<Render::SwapChainSurface>(
            inDevice, GameWindowing::Internal::NativeHandle(window),
            GameWindowing::Internal::FramebufferSize(window).x, GameWindowing::Internal::FramebufferSize(window).y))
        , viewport(new GameViewport(inClient, *swapChainSurface))
    {
    }

    GameWindow::~GameWindow()
    {
        WaitIdle();
        viewport.Reset();
        swapChainSurface.Reset();
        glfwDestroyWindow(window);
        if (--GameWindowing::Internal::windowCount == 0) {
            glfwTerminate();
        }
    }

    bool GameWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(window) == GLFW_TRUE;
    }

    void GameWindow::RequestClose() const
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void GameWindow::Tick(float inDeltaSeconds)
    {
        glfwPollEvents();
        const auto size = GameWindowing::Internal::FramebufferSize(window);
        if (size != swapChainSurface->GetSize()) {
            WaitIdle();
            swapChainSurface->Resize(size.x, size.y);
        }
    }

    void GameWindow::Render(float inDeltaSeconds)
    {
        renderThread.EmplaceTask([this]() -> void { swapChainSurface->AcquireBackTexture(); });
        viewport->Render(inDeltaSeconds);
        renderThread.EmplaceTask([this]() -> void { swapChainSurface->Present(); });
    }

    void GameWindow::WaitIdle() const
    {
        renderThread.Flush();
        swapChainSurface->WaitRenderingIdle();
    }
}
