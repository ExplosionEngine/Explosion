#include <algorithm>

#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui.h>

#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

#include <Core/Paths.h>
#include <Editor/Theme/Theme.h>
#include <Editor/Widget/TablerIcons.h>
#include <Editor/ImGui/ImGuiWindow.h>
#include <Runtime/Engine.h>

namespace Editor::WindowUi::Internal {
    constexpr float fontSize = 15.0f;
    uint32_t windowCount = 0;

    static GLFWwindow* CreateGlfwWindow(const ImGuiWindowDesc& inDesc)
    {
        if (windowCount == 0) {
            const auto initialized = glfwInit();
            Assert(initialized == GLFW_TRUE);
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_MAXIMIZED, inDesc.maximized ? GLFW_TRUE : GLFW_FALSE);
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

    static void LoadIconFonts(ImFontAtlas& inFontAtlas)
    {
        static ImVector<ImWchar> glyphRanges;
        if (glyphRanges.empty()) {
            ImFontGlyphRangesBuilder builder;
            builder.AddText(Icons::Tabler::allGlyphs);
            builder.BuildRanges(&glyphRanges);
        }
        const auto fontPath = (Core::Paths::EngineResDir() / "Editor" / "Icons" / "Tabler" / "tabler-icons.ttf").String();
        ImFontConfig config;
        config.MergeMode = true;
        config.PixelSnapH = true;
        config.GlyphMinAdvanceX = fontSize;
        config.GlyphMaxAdvanceX = fontSize;
        Assert(inFontAtlas.AddFontFromFileTTF(fontPath.c_str(), fontSize, &config, glyphRanges.Data) != nullptr);
    }
}

namespace Editor {
    ImGuiWindow::ImGuiWindow(ImGuiWindowDesc inDesc)
        : desc(std::move(inDesc))
        , window(WindowUi::Internal::CreateGlfwWindow(desc))
        , renderThread(Runtime::EngineHolder::Get().GetRenderModule().GetRenderThread())
        , swapChainSurface(Common::MakeUnique<Render::SwapChainSurface>(
            *Runtime::EngineHolder::Get().GetRenderModule().GetDevice(), WindowUi::Internal::NativeHandle(window),
            WindowUi::Internal::FramebufferSize(window).x, WindowUi::Internal::FramebufferSize(window).y))
        , imguiContext(nullptr)
    {
        InitializeImGui();
        imguiPlatform = Common::MakeUnique<ImGuiPlatformBackend>(window, desc);
        imguiRenderer = Common::MakeUnique<ImGuiRhiRenderer>(swapChainSurface->GetDevice(), *swapChainSurface, renderThread);
    }

    ImGuiWindow::~ImGuiWindow()
    {
        ImGui::SetCurrentContext(imguiContext);
        WaitIdle();
        imguiPlatform.Reset();
        imguiRenderer.Reset();
        swapChainSurface.Reset();
        ImGui::DestroyContext(imguiContext);
        glfwDestroyWindow(window);
        if (--WindowUi::Internal::windowCount == 0) {
            glfwTerminate();
        }
    }

    bool ImGuiWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(window) == GLFW_TRUE;
    }

    void ImGuiWindow::RequestClose() const
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void ImGuiWindow::Tick(float inDeltaSeconds)
    {
        glfwPollEvents();
        ImGui::SetCurrentContext(imguiContext);
        const auto size = WindowUi::Internal::FramebufferSize(window);
        if (size != swapChainSurface->GetSize()) {
            WaitIdle();
            swapChainSurface->Resize(size.x, size.y);
        }
        imguiRenderer->NewFrame();
        imguiPlatform->NewFrame(inDeltaSeconds);
        ImGui::NewFrame();
        RenderContents();
    }

    void ImGuiWindow::Render(float inDeltaSeconds)
    {
        ImGui::SetCurrentContext(imguiContext);
        ImGui::Render();
        renderThread.EmplaceTask([this]() -> void {
            imguiRenderer->WaitForPreviousFrame();
            swapChainSurface->AcquireBackTexture();
        });
        imguiRenderer->Submit(*ImGui::GetDrawData());
        renderThread.EmplaceTask([this]() -> void {
            swapChainSurface->Present();
        });
    }

    void ImGuiWindow::WaitIdle() const
    {
        imguiRenderer->WaitIdle();
        swapChainSurface->WaitRenderingIdle();
    }

    GLFWwindow* ImGuiWindow::GetNativeWindow() const
    {
        return window;
    }

    Render::SwapChainSurface& ImGuiWindow::GetSwapChainSurface()
    {
        return *swapChainSurface;
    }

    void ImGuiWindow::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        imguiContext = ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        if (!desc.layoutFile.Empty()) {
            desc.layoutFile.Parent().MakeDir();
            imguiIniFilename = desc.layoutFile.String();
            io.IniFilename = imguiIniFilename.c_str();
        } else {
            io.IniFilename = nullptr;
        }
        ImFontConfig fontConfig;
        fontConfig.SizePixels = WindowUi::Internal::fontSize;
        io.FontDefault = io.Fonts->AddFontDefaultVector(&fontConfig);
        WindowUi::Internal::LoadIconFonts(*io.Fonts);
        ThemeSwitcher::ApplyCurrentTheme();
    }
}
