#include <imgui.h>
#include <imgui_internal.h>

#include <Editor/EditorContext.h>
#include <Editor/Frame/EditorFrame.h>
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
    EditorFrame::EditorFrame(EditorContext& inContext, Runtime::Canvas& inSceneRenderCanvas, EditorLogStream& inLogStream, Common::Path inAssetDirectory)
        : context(inContext)
    {
        tabs.Emplace<SceneTab>(context.GetSceneClient(), inSceneRenderCanvas);
        tabs.Emplace<OutlinerTab>(context);
        tabs.Emplace<InspectorTab>(context);
        tabs.Emplace<AssetsTab>(std::move(inAssetDirectory));
        tabs.Emplace<LogTab>(inLogStream);
    }

    EditorFrame::~EditorFrame() = default;

    void EditorFrame::Render(bool& outRequestQuit)
    {
        DrawDockSpace();
        RenderMenuBar(outRequestQuit);
        tabs.Render();
    }

    void EditorFrame::RenderMenuBar(bool& outRequestQuit)
    {
        if (!ImGui::BeginMainMenuBar()) {
            return;
        }
        const std::string fileMenuLabel = Widgets::Label(Icons::Tabler::file, "File");
        if (ImGui::BeginMenu(fileMenuLabel.c_str())) {
            const std::string saveLevelLabel = Widgets::Label(Icons::Tabler::deviceFloppy, "Save Level");
            if (ImGui::MenuItem(saveLevelLabel.c_str(), "Ctrl+S")) {
                context.GetSceneClient().SaveLevel();
            }
            ImGui::Separator();
            const std::string exitLabel = Widgets::Label(Icons::Tabler::doorExit, "Exit");
            if (ImGui::MenuItem(exitLabel.c_str())) {
                outRequestQuit = true;
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

    void EditorFrame::DrawDockSpace() const
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
}
