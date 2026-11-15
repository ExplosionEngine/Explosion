//
// Created by johnk on 2026/7/7.
//

#pragma once

#include <Common/Memory.h>
#include <Editor/EditorContext.h>
#include <Editor/EditorWindow.h>
#include <Editor/Frame/EditorFrame.h>
#include <Editor/Frame/ProjectHubFrame.h>
#include <Editor/ImGui/ImGuiPlatformBackend.h>
#include <Editor/ImGui/ImGuiRhiRenderer.h>
#include <Runtime/Canvas.h>

namespace Editor {
    enum class EditorApplicationMode : uint8_t {
        projectHub,
        editor
    };

    struct EditorApplicationDesc {
        EditorApplicationMode mode;
        std::string rhiType;
        bool gpuDebug;
        bool softwareGpu;
        std::string projectRoot;
    };

    class EditorApplication final {
    public:
        explicit EditorApplication(EditorApplicationDesc inDesc);
        ~EditorApplication();

        NonCopyable(EditorApplication)
        NonMovable(EditorApplication)

        int Run();

    private:
        void InitializeImGui();
        void ShutdownImGui();
        void InstallCallbacks();
        void RenderEditorFrame();
        void HandleKey(int inKey, int inAction);
        void HandleMouseButton(int inButton, int inAction);
        void HandleCursor(double inX, double inY);
        float NextDeltaSeconds();

        EditorApplicationDesc desc;
        std::string imguiIniFilename;
        Common::UniquePtr<EditorWindow> window;
        Common::UniquePtr<ImGuiPlatformBackend> imguiPlatform;
        Common::UniquePtr<ImGuiRhiRenderer> imguiRenderer;
        Common::UniquePtr<EditorContext> context;
        Common::UniquePtr<Runtime::Canvas> sceneRenderCanvas;
        Common::UniquePtr<ProjectService> projectService;
        Common::UniquePtr<RecentProjectStore> recentProjects;
        Common::UniquePtr<ProjectLauncher> projectLauncher;
        Common::UniquePtr<ProjectHubFrame> projectHubFrame;
        Common::UniquePtr<EditorFrame> editorFrame;
        double lastFrameSeconds;
        double lastCursorX;
        double lastCursorY;
        bool hasLastCursor;
        bool requestQuit;
    };
}
