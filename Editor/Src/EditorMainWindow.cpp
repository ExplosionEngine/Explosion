#include <imgui.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

#include <Core/Paths.h>
#include <Editor/EditorContext.h>
#include <Editor/EditorMainWindow.h>
#include <Editor/Tab/AssetsTab.h>
#include <Editor/Tab/InspectorTab.h>
#include <Editor/Tab/LogTab.h>
#include <Editor/Tab/OutlinerTab.h>
#include <Editor/Tab/SceneTab.h>
#include <Editor/Widget/IconWidgets.h>
#include <Editor/Widget/TablerIcons.h>

namespace Editor::Workspace::Internal {
    constexpr const char* editorDockSpaceName = "EditorDockSpace";
    constexpr const char* editorDockSpaceId = "EditorDockSpaceV3";
}

namespace Editor {
    EditorMainWindow::EditorMainWindow(EditorContext& inContext, EditorLogStream& inLogStream, Common::Path inAssetDirectory)
        : ImGuiWindow(ImGuiWindowDesc {
            .title = Core::Paths::GameRootDir().DirName() + " - Explosion Editor",
            .width = 1600,
            .height = 900,
            .maximized = true,
            .layoutFile = Core::Paths::GameCacheDir() / "Editor" / "Layout" / "Main.ini",
            .onKey = [this](int inKey, int, int inAction, int) -> void { HandleKey(inKey, inAction); },
            .onMouseButton = [this](int inButton, int inAction, int) -> void { HandleMouseButton(inButton, inAction); },
            .onCursor = [this](double inX, double inY) -> void { HandleCursor(inX, inY); },
            .onFocus = [this](bool inFocused) -> void {
                if (!inFocused) {
                    sceneViewport.EndCameraLook();
                    hasLastCursor = false;
                }
            }
        })
        , context(inContext)
        , lastCursorX(0.0)
        , lastCursorY(0.0)
        , hasLastCursor(false)
        , sceneViewport(context.GetWorld(), GetNativeWindow(), GetSwapChainSurface().GetDevice())
    {
        tabs.Emplace<SceneTab>(sceneViewport);
        tabs.Emplace<OutlinerTab>(context);
        tabs.Emplace<InspectorTab>(context);
        tabs.Emplace<AssetsTab>(std::move(inAssetDirectory));
        tabs.Emplace<LogTab>(inLogStream);
    }

    EditorMainWindow::~EditorMainWindow()
    {
        WaitIdle();
    }

    void EditorMainWindow::RenderContents()
    {
        DrawDockSpace();
        RenderMenuBar();
        tabs.Render();
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            context.SaveLevel();
        }
    }

    void EditorMainWindow::RenderMenuBar()
    {
        if (!ImGui::BeginMainMenuBar()) {
            return;
        }
        const std::string fileMenuLabel = Widgets::Label(Icons::Tabler::file, "File");
        if (ImGui::BeginMenu(fileMenuLabel.c_str())) {
            const std::string saveLevelLabel = Widgets::Label(Icons::Tabler::deviceFloppy, "Save Level");
            if (ImGui::MenuItem(saveLevelLabel.c_str(), "Ctrl+S")) {
                context.SaveLevel();
            }
            ImGui::Separator();
            const std::string exitLabel = Widgets::Label(Icons::Tabler::doorExit, "Exit");
            if (ImGui::MenuItem(exitLabel.c_str())) {
                RequestClose();
            }
            ImGui::EndMenu();
        }
        const std::string viewMenuLabel = Widgets::Label(Icons::Tabler::layoutDashboard, "View");
        if (ImGui::BeginMenu(viewMenuLabel.c_str())) {
            tabs.RenderViewMenuItems();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    void EditorMainWindow::DrawDockSpace() const
    {
        const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(mainViewport->WorkPos);
        ImGui::SetNextWindowSize(mainViewport->WorkSize);
        ImGui::SetNextWindowViewport(mainViewport->ID);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavFocus
            | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(Workspace::Internal::editorDockSpaceName, nullptr, windowFlags);
        ImGui::PopStyleVar(3);

        const ImGuiID dockSpaceId = ImGui::GetID(Workspace::Internal::editorDockSpaceId);
        tabs.BuildInitialDockLayout(dockSpaceId, mainViewport->WorkSize);
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        ImGui::End();
    }

    void EditorMainWindow::Render(float inDeltaSeconds)
    {
        sceneViewport.Render(inDeltaSeconds);
        ImGuiWindow::Render(inDeltaSeconds);
    }

    void EditorMainWindow::HandleKey(int inKey, int inAction)
    {
        if (inKey == GLFW_KEY_ESCAPE && inAction == GLFW_PRESS) {
            sceneViewport.EndCameraLook();
        }
        if (inAction == GLFW_PRESS || inAction == GLFW_RELEASE) {
            const bool routeToScene = sceneViewport.IsCameraLooking()
                || (!ImGui::GetIO().WantCaptureKeyboard && sceneViewport.IsHovered());
            if (routeToScene || inAction == GLFW_RELEASE) {
                sceneViewport.OnKey(inKey, inAction == GLFW_PRESS);
            }
        }
    }

    void EditorMainWindow::HandleMouseButton(int inButton, int inAction)
    {
        if (inButton != GLFW_MOUSE_BUTTON_RIGHT) {
            return;
        }
        if (inAction == GLFW_PRESS && sceneViewport.IsHovered()) {
            hasLastCursor = false;
            sceneViewport.BeginCameraLook();
        } else if (inAction == GLFW_RELEASE) {
            sceneViewport.EndCameraLook();
        }
    }

    void EditorMainWindow::HandleCursor(double inX, double inY)
    {
        if (sceneViewport.IsCameraLooking() && hasLastCursor) {
            sceneViewport.AddCameraLookDelta(static_cast<float>(inX - lastCursorX), static_cast<float>(inY - lastCursorY));
        }
        lastCursorX = inX;
        lastCursorY = inY;
        hasLastCursor = true;
    }
}
