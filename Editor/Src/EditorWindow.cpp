//
// Created by johnk on 2026/7/7.
//

#include <algorithm>
#include <cstdint>

#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#if PLATFORM_WINDOWS
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif
#endif

#include <Editor/EditorWindow.h>
#include <Render/RenderThread.h>

namespace Editor::Windowing::Internal {
    static uint32_t ClampExtent(int inValue)
    {
        return static_cast<uint32_t>(std::max(1, inValue));
    }
}

namespace Editor {
    EditorWindow::EditorWindow(RHI::Device& inDevice, Render::RenderThread& inRenderThread, const EditorWindowDesc& inDesc)
        : Runtime::Window(inDevice)
        , renderThread(inRenderThread)
        , window(nullptr)
        , framebufferWidth(inDesc.width)
        , framebufferHeight(inDesc.height)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_MAXIMIZED, inDesc.maximized ? GLFW_TRUE : GLFW_FALSE);
        window = glfwCreateWindow(
            static_cast<int>(inDesc.width),
            static_cast<int>(inDesc.height),
            inDesc.title.c_str(),
            nullptr,
            nullptr);
        Assert(window != nullptr);

        int fbWidth = 0;
        int fbHeight = 0;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        framebufferWidth = Windowing::Internal::ClampExtent(fbWidth);
        framebufferHeight = Windowing::Internal::ClampExtent(fbHeight);

        InitializeSurface(
            GetDevice().CreateSurface(RHI::SurfaceCreateInfo(GetPlatformWindow())),
            framebufferWidth,
            framebufferHeight);
        InstallCallbacks();
    }

    EditorWindow::~EditorWindow()
    {
        WaitRenderingIdle();
        DestroySurface();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWwindow* EditorWindow::GetNativeWindow() const
    {
        return window;
    }

    uint32_t EditorWindow::GetWidth() const
    {
        return GetFramebufferWidth();
    }

    uint32_t EditorWindow::GetHeight() const
    {
        return GetFramebufferHeight();
    }

    uint32_t EditorWindow::GetFramebufferWidth() const
    {
        return framebufferWidth;
    }

    uint32_t EditorWindow::GetFramebufferHeight() const
    {
        return framebufferHeight;
    }

    bool EditorWindow::ShouldClose() const
    {
        return static_cast<bool>(glfwWindowShouldClose(window));
    }

    void EditorWindow::RequestClose() const
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void EditorWindow::PollEvents() const
    {
        glfwPollEvents();
    }

    void EditorWindow::SetTitle(const std::string& inTitle) const
    {
        glfwSetWindowTitle(window, inTitle.c_str());
    }

    void EditorWindow::SetCursorCaptured(bool inCaptured) const
    {
        glfwSetInputMode(window, GLFW_CURSOR, inCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    void EditorWindow::RecreateSwapChainIfNeeded()
    {
        int fbWidth = 0;
        int fbHeight = 0;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        const uint32_t newWidth = Windowing::Internal::ClampExtent(fbWidth);
        const uint32_t newHeight = Windowing::Internal::ClampExtent(fbHeight);
        if (newWidth == framebufferWidth && newHeight == framebufferHeight) {
            return;
        }

        framebufferWidth = newWidth;
        framebufferHeight = newHeight;
        WaitRenderingIdle();
        Runtime::Window::Resize(framebufferWidth, framebufferHeight);
    }

    void EditorWindow::WaitRenderingIdle() const
    {
        renderThread.Flush();
        Runtime::Window::WaitRenderingIdle();
    }

    void EditorWindow::InstallCallbacks()
    {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, [](GLFWwindow* inWindow, int inKey, int inScancode, int inAction, int inMods) -> void {
            static_cast<EditorWindow*>(glfwGetWindowUserPointer(inWindow))->onKey.Broadcast(inKey, inScancode, inAction, inMods);
        });
        glfwSetCharCallback(window, [](GLFWwindow* inWindow, unsigned int inCodepoint) -> void {
            static_cast<EditorWindow*>(glfwGetWindowUserPointer(inWindow))->onChar.Broadcast(inCodepoint);
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* inWindow, int inButton, int inAction, int inMods) -> void {
            static_cast<EditorWindow*>(glfwGetWindowUserPointer(inWindow))->onMouseButton.Broadcast(inButton, inAction, inMods);
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow* inWindow, double inX, double inY) -> void {
            static_cast<EditorWindow*>(glfwGetWindowUserPointer(inWindow))->onCursor.Broadcast(inX, inY);
        });
        glfwSetScrollCallback(window, [](GLFWwindow* inWindow, double inX, double inY) -> void {
            static_cast<EditorWindow*>(glfwGetWindowUserPointer(inWindow))->onScroll.Broadcast(inX, inY);
        });
        glfwSetWindowFocusCallback(window, [](GLFWwindow* inWindow, int inFocused) -> void {
            static_cast<EditorWindow*>(glfwGetWindowUserPointer(inWindow))->onFocus.Broadcast(inFocused == GLFW_TRUE);
        });
    }

    void* EditorWindow::GetPlatformWindow() const
    {
#if PLATFORM_WINDOWS
        return glfwGetWin32Window(window);
#elif PLATFORM_MACOS
        return glfwGetCocoaView(window);
#else
        Unimplement();
        return nullptr;
#endif
    }

}
