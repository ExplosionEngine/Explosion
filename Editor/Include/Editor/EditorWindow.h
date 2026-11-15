#pragma once

#include <string>

#include <Common/Delegate.h>
#include <Runtime/Window.h>

struct GLFWwindow;

namespace Render {
    class RenderThread;
}

namespace Editor {
    struct EditorWindowDesc {
        std::string title;
        uint32_t width;
        uint32_t height;
        bool maximized;
    };

    class EditorWindow final : public Runtime::Window {
    public:
        EditorWindow(RHI::Device& inDevice, Render::RenderThread& inRenderThread, const EditorWindowDesc& inDesc);
        ~EditorWindow() override;

        NonCopyable(EditorWindow)
        NonMovable(EditorWindow)

        GLFWwindow* GetNativeWindow() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetFramebufferWidth() const;
        uint32_t GetFramebufferHeight() const;
        bool ShouldClose() const;
        void RequestClose() const;
        void PollEvents() const;
        void SetTitle(const std::string& inTitle) const;
        void SetCursorCaptured(bool inCaptured) const;
        void RecreateSwapChainIfNeeded();
        void WaitRenderingIdle() const;

        Common::Delegate<int, int, int, int> onKey;
        Common::Delegate<uint32_t> onChar;
        Common::Delegate<int, int, int> onMouseButton;
        Common::Delegate<double, double> onCursor;
        Common::Delegate<double, double> onScroll;
        Common::Delegate<bool> onFocus;

    private:
        void InstallCallbacks();
        void* GetPlatformWindow() const;

        Render::RenderThread& renderThread;
        GLFWwindow* window;
        uint32_t framebufferWidth;
        uint32_t framebufferHeight;
    };
}
