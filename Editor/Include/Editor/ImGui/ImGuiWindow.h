#pragma once

#include <functional>
#include <string>

#include <Common/FileSystem.h>
#include <Editor/ImGui/ImGuiPlatformBackend.h>
#include <Editor/ImGui/ImGuiRhiRenderer.h>
#include <Render/SwapChainSurface.h>
#include <Runtime/Window.h>

struct GLFWwindow;
struct ImGuiContext;

namespace Editor {
    struct ImGuiWindowDesc {
        std::string title;
        uint32_t width;
        uint32_t height;
        bool maximized = false;
        Common::Path layoutFile;
        std::function<void(int, int, int, int)> onKey;
        std::function<void(uint32_t)> onChar;
        std::function<void(int, int, int)> onMouseButton;
        std::function<void(double, double)> onCursor;
        std::function<void(double, double)> onScroll;
        std::function<void(bool)> onFocus;
    };

    class ImGuiWindow : public Runtime::Window {
    public:
        ~ImGuiWindow() override;

        bool ShouldClose() const override;
        void RequestClose() const override;
        void Tick(float inDeltaSeconds) override;
        void Render(float inDeltaSeconds) override;
        void WaitIdle() const override;

    protected:
        explicit ImGuiWindow(ImGuiWindowDesc inDesc);

        GLFWwindow* GetNativeWindow() const;
        Render::SwapChainSurface& GetSwapChainSurface();
        virtual void RenderContents() = 0;

    private:
        void InitializeImGui();

        ImGuiWindowDesc desc;
        GLFWwindow* window;
        Render::RenderThread& renderThread;
        Common::UniquePtr<Render::SwapChainSurface> swapChainSurface;
        ImGuiContext* imguiContext;
        std::string imguiIniFilename;
        Common::UniquePtr<ImGuiPlatformBackend> imguiPlatform;
        Common::UniquePtr<ImGuiRhiRenderer> imguiRenderer;
    };
}
