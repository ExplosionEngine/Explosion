//
// Created by johnk on 2026/7/7.
//

#include <format>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <Common/Debug.h>
#include <Common/Time.h>
#include <Core/Paths.h>
#include <Editor/EditorApplication.h>
#include <Editor/EditorLog.h>
#include <Editor/Theme/Theme.h>
#include <Editor/Widget/TablerIcons.h>
#include <Runtime/Engine.h>

namespace Editor::Internal {
    constexpr uint32_t defaultWindowWidth = 1600;
    constexpr uint32_t defaultWindowHeight = 900;
    constexpr uint32_t projectHubWindowWidth = 560;
    constexpr uint32_t projectHubWindowHeight = 720;
    constexpr float defaultFontSize = 15.0f;
    constexpr uint32_t defaultSceneWidth = 1280;
    constexpr uint32_t defaultSceneHeight = 720;
    constexpr RHI::PixelFormat sceneColorFormat = RHI::PixelFormat::rgba8Unorm;

    static Runtime::CanvasDesc CreateSceneRenderCanvasDesc()
    {
        Runtime::CanvasDesc result;
        result.width = defaultSceneWidth;
        result.height = defaultSceneHeight;
        result.format = sceneColorFormat;
        result.debugName = "editorSceneColor";
        return result;
    }

    static std::string MakeWindowTitle(const EditorApplicationDesc& inDesc)
    {
        if (inDesc.mode == EditorApplicationMode::projectHub) {
            return "Explosion Project Hub";
        }
        const std::string projectName = Core::Paths::HasSetGameRoot()
            ? Core::Paths::GameRootDir().DirName()
            : "Untitled";
        return std::format("{} - Explosion Editor", projectName);
    }

    static uint32_t GetInitialWindowWidth(const EditorApplicationDesc& inDesc)
    {
        return inDesc.mode == EditorApplicationMode::projectHub ? projectHubWindowWidth : defaultWindowWidth;
    }

    static uint32_t GetInitialWindowHeight(const EditorApplicationDesc& inDesc)
    {
        return inDesc.mode == EditorApplicationMode::projectHub ? projectHubWindowHeight : defaultWindowHeight;
    }

    static void LoadIconFonts(ImFontAtlas& inFontAtlas, float inSizePixels)
    {
        static ImVector<ImWchar> glyphRanges;
        if (glyphRanges.empty()) {
            ImFontGlyphRangesBuilder builder;
            builder.AddText(Icons::Tabler::allGlyphs);
            builder.BuildRanges(&glyphRanges);
        }

        const std::string fontPath = (Core::Paths::EngineResDir() / "Editor" / "Icons" / "Tabler" / "tabler-icons.ttf").String();
        ImFontConfig config;
        config.MergeMode = true;
        config.PixelSnapH = true;
        config.GlyphMinAdvanceX = inSizePixels;
        config.GlyphMaxAdvanceX = inSizePixels;
        Assert(inFontAtlas.AddFontFromFileTTF(fontPath.c_str(), inSizePixels, &config, glyphRanges.Data) != nullptr);
    }
}

namespace Editor {
    EditorApplication::EditorApplication(EditorApplicationDesc inDesc)
        : desc(std::move(inDesc))
        , lastFrameSeconds(Common::TimePoint::Now().ToSeconds())
        , lastCursorX(0.0)
        , lastCursorY(0.0)
        , hasLastCursor(false)
        , requestQuit(false)
    {
        InitializeImGui();
        auto& renderModule = Runtime::EngineHolder::Get().GetRenderModule();
        window = Common::MakeUnique<EditorWindow>(
            *renderModule.GetDevice(), renderModule.GetRenderThread(),
            EditorWindowDesc {
                .title = Internal::MakeWindowTitle(desc),
                .width = Internal::GetInitialWindowWidth(desc),
                .height = Internal::GetInitialWindowHeight(desc),
                .maximized = desc.mode == EditorApplicationMode::editor
            });
        imguiPlatform = Common::MakeUnique<ImGuiPlatformBackend>(*window);
        imguiRenderer = Common::MakeUnique<ImGuiRhiRenderer>(*window, renderModule.GetRenderThread());
        InstallCallbacks();

        if (desc.mode == EditorApplicationMode::editor) {
            context = Common::MakeUnique<EditorContext>();
            auto& sceneClient = context->GetSceneClient();
            sceneClient.SetEditorWindow(*window);
            sceneRenderCanvas = Common::MakeUnique<Runtime::Canvas>(window->GetDevice(), Internal::CreateSceneRenderCanvasDesc());
            sceneClient.SetRenderSurface(sceneRenderCanvas.Get());
            sceneClient.OpenProjectLevel();
            editorFrame = Common::MakeUnique<EditorFrame>(*context, *sceneRenderCanvas, EditorLogStream::Get(), Core::Paths::GameAssetDir());
        } else {
            projectService = Common::MakeUnique<ProjectService>(Core::Paths::EngineResDir() / "Editor" / "ProjectTemplates");
            recentProjects = Common::MakeUnique<RecentProjectStore>(Core::Paths::EngineCacheDir() / "Editor" / "ProjectHub" / "RecentProjects.json");
            projectLauncher = Common::MakeUnique<ProjectLauncher>(EditorLaunchOptions {
                .executablePath = Core::Paths::ExecutablePath().String(),
                .rhiType = desc.rhiType,
                .gpuDebug = desc.gpuDebug,
                .softwareGpu = desc.softwareGpu
            });
            projectHubFrame = Common::MakeUnique<ProjectHubFrame>(*projectService, *recentProjects, *projectLauncher);
        }
    }

    EditorApplication::~EditorApplication()
    {
        if (context != nullptr) {
            context->GetSceneClient().SetRenderSurface(nullptr);
        }
        imguiRenderer->WaitIdle();
        editorFrame.Reset();
        context.Reset();
        sceneRenderCanvas.Reset();
        projectHubFrame.Reset();
        imguiPlatform.Reset();
        imguiRenderer.Reset();
        window.Reset();
        ShutdownImGui();
    }

    int EditorApplication::Run()
    {
        while (!window->ShouldClose()) {
            window->PollEvents();
            window->RecreateSwapChainIfNeeded();
            const float deltaSeconds = NextDeltaSeconds();
            imguiRenderer->NewFrame();
            imguiPlatform->NewFrame(deltaSeconds);
            ImGui::NewFrame();

            if (desc.mode == EditorApplicationMode::editor) {
                RenderEditorFrame();
            } else {
                requestQuit = projectHubFrame->Render();
            }
            if (requestQuit) {
                window->RequestClose();
            }
            ImGui::Render();
            Runtime::EngineHolder::Get().Tick(deltaSeconds);
            imguiRenderer->Submit(*ImGui::GetDrawData());
        }
        return 0;
    }

    void EditorApplication::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        if (desc.mode == EditorApplicationMode::editor) {
            const Common::Path iniFilePath = Core::Paths::GameCacheDir() / "Editor" / "Layout" / "Main.ini";
            iniFilePath.Parent().MakeDir();
            imguiIniFilename = iniFilePath.String();
            io.IniFilename = imguiIniFilename.c_str();
        } else {
            io.IniFilename = nullptr;
        }
        ImFontConfig defaultFontConfig;
        defaultFontConfig.SizePixels = Internal::defaultFontSize;
        io.FontDefault = io.Fonts->AddFontDefaultVector(&defaultFontConfig);
        Internal::LoadIconFonts(*io.Fonts, Internal::defaultFontSize);
        ThemeSwitcher::ApplyCurrentTheme();
    }

    void EditorApplication::ShutdownImGui()
    {
        ImGui::DestroyContext();
    }

    void EditorApplication::InstallCallbacks()
    {
        window->onKey.BindLambda([this](int inKey, int, int inAction, int) -> void {
            HandleKey(inKey, inAction);
        });
        window->onMouseButton.BindLambda([this](int inButton, int inAction, int) -> void {
            HandleMouseButton(inButton, inAction);
        });
        window->onCursor.BindMember<&EditorApplication::HandleCursor>(*this);
        window->onFocus.BindLambda([this](bool inFocused) -> void {
            if (!inFocused && context != nullptr) {
                context->GetSceneClient().EndCameraLook();
                hasLastCursor = false;
            }
        });
    }

    void EditorApplication::RenderEditorFrame()
    {
        editorFrame->Render(requestQuit);
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            context->GetSceneClient().SaveLevel();
        }
    }

    void EditorApplication::HandleKey(int inKey, int inAction)
    {
        const ImGuiIO& io = ImGui::GetIO();
        if (context != nullptr) {
            auto& sceneClient = context->GetSceneClient();
            if (inKey == GLFW_KEY_ESCAPE && inAction == GLFW_PRESS) {
                sceneClient.EndCameraLook();
            }
            if (inAction == GLFW_PRESS || inAction == GLFW_RELEASE) {
                const bool canRouteToScene = sceneClient.IsCameraLooking() || (!io.WantCaptureKeyboard && sceneClient.IsSceneHovered());
                if (canRouteToScene || inAction == GLFW_RELEASE) {
                    sceneClient.OnKey(inKey, inAction == GLFW_PRESS);
                }
            }
        }
    }

    void EditorApplication::HandleMouseButton(int inButton, int inAction)
    {
        if (context == nullptr || inButton != GLFW_MOUSE_BUTTON_RIGHT) {
            return;
        }
        auto& sceneClient = context->GetSceneClient();
        if (inAction == GLFW_PRESS && sceneClient.IsSceneHovered()) {
            hasLastCursor = false;
            sceneClient.BeginCameraLook();
        } else if (inAction == GLFW_RELEASE) {
            sceneClient.EndCameraLook();
        }
    }

    void EditorApplication::HandleCursor(double inX, double inY)
    {
        if (context != nullptr) {
            auto& sceneClient = context->GetSceneClient();
            if (sceneClient.IsCameraLooking() && hasLastCursor) {
                sceneClient.AddCameraLookDelta(static_cast<float>(inX - lastCursorX), static_cast<float>(inY - lastCursorY));
            }
        }
        lastCursorX = inX;
        lastCursorY = inY;
        hasLastCursor = true;
    }

    float EditorApplication::NextDeltaSeconds()
    {
        const double now = Common::TimePoint::Now().ToSeconds();
        const float deltaSeconds = static_cast<float>(now - lastFrameSeconds);
        lastFrameSeconds = now;
        return deltaSeconds;
    }
}
