//
// Created by johnk on 2026/7/8.
//

#include <format>
#include <ranges>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Editor/EditorLog.h>
#include <Editor/ProjectHubWindow.h>
#include <Editor/Utils/PlatformUtils.h>
#include <Editor/Widget/IconWidgets.h>
#include <Editor/Widget/TablerIcons.h>

namespace Editor::ProjectHub::Internal {
    constexpr float actionButtonHeight = 58.0f;
    constexpr float recentProjectHeight = 68.0f;
    constexpr float contentSpacing = 12.0f;

    static bool RenderRecentProject(const RecentProjectInfo& inProject)
    {
        ImGui::PushID(inProject.path.c_str());
        const ImVec2 cardMin = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##RecentProject", ImVec2(ImGui::GetContentRegionAvail().x, recentProjectHeight));
        const ImVec2 cardMax = ImGui::GetItemRectMax();
        const bool hovered = ImGui::IsItemHovered();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(cardMin, cardMax, ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 8.0f);
        drawList->AddText(ImVec2(cardMin.x + 16.0f, cardMin.y + 26.0f), ImGui::GetColorU32(ImGuiCol_CheckMark), Icons::Tabler::folder);
        drawList->PushClipRect(ImVec2(cardMin.x + 46.0f, cardMin.y), ImVec2(cardMax.x - 38.0f, cardMax.y), true);
        drawList->AddText(ImVec2(cardMin.x + 46.0f, cardMin.y + 13.0f), ImGui::GetColorU32(ImGuiCol_Text), inProject.name.c_str());
        drawList->AddText(ImVec2(cardMin.x + 46.0f, cardMin.y + 38.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), inProject.path.c_str());
        drawList->PopClipRect();
        drawList->AddText(ImVec2(cardMax.x - 25.0f, cardMin.y + 26.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), Icons::Tabler::chevronRight);

        ImGui::PopID();
        return ImGui::IsItemClicked();
    }
}

namespace Editor {
    ProjectHubWindow::ProjectHubWindow(ProjectService& inProjectService, RecentProjectStore& inRecentProjects, ProjectLauncher& inProjectLauncher)
        : ImGuiWindow(ImGuiWindowDesc { .title = "Explosion Project Hub", .width = 560, .height = 720 })
        , projectService(inProjectService)
        , recentProjects(inRecentProjects)
        , projectLauncher(inProjectLauncher)
        , selectedTemplateIndex(0)
    {
        auto templatesResult = projectService.ListTemplates();
        if (templatesResult.IsOk()) {
            projectTemplates = std::move(templatesResult.Value());
        } else {
            statusMessage = templatesResult.Error();
        }
        if (auto result = recentProjects.Load(); result.IsErr()) {
            if (!statusMessage.empty()) {
                statusMessage += "\n";
            }
            statusMessage += result.Error();
        }
    }

    ProjectHubWindow::~ProjectHubWindow() = default;

    void ProjectHubWindow::RenderContents()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(26.0f, 24.0f));
        ImGui::Begin(
            "##ProjectHub",
            nullptr,
            ImGuiWindowFlags_NoDecoration
                | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoSavedSettings
                | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar(2);

        RenderActionBar();
        ImGui::Dummy(ImVec2(0.0f, 22.0f));
        RenderRecentProjects();
        RenderCreateProjectPopup();
        ImGui::End();
    }

    void ProjectHubWindow::RenderActionBar()
    {
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float buttonWidth = (ImGui::GetContentRegionAvail().x - spacing) * 0.5f;
        const std::string openLabel = Widgets::Label(Icons::Tabler::folderOpen, "Open");
        if (Widgets::PrimaryButton(openLabel.c_str(), ImVec2(buttonWidth, ProjectHub::Internal::actionButtonHeight))) {
            if (const auto selectedDirectory = PlatformUtils::SelectDirectory("Open Explosion Project")) {
                OpenProject(Common::Path(*selectedDirectory));
            }
        }

        ImGui::SameLine();
        const std::string createLabel = Widgets::Label(Icons::Tabler::plus, "Create");
        if (ImGui::Button(createLabel.c_str(), ImVec2(buttonWidth, ProjectHub::Internal::actionButtonHeight))) {
            statusMessage.clear();
            ImGui::OpenPopup("##CreateProjectPopup");
        }
    }

    void ProjectHubWindow::RenderRecentProjects()
    {
        const std::string recentProjectsLabel = Widgets::Label(Icons::Tabler::folder, "Recent projects");
        ImGui::TextUnformatted(recentProjectsLabel.c_str());
        const std::string projectCount = std::to_string(recentProjects.Projects().size());
        ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(projectCount.c_str()).x);
        ImGui::TextDisabled("%s", projectCount.c_str());
        ImGui::Dummy(ImVec2(0.0f, 7.0f));

        if (!statusMessage.empty()) {
            const std::string statusLabel = Widgets::Label(Icons::Tabler::circleX, statusMessage);
            Widgets::PushErrorTextColor();
            ImGui::TextWrapped("%s", statusLabel.c_str());
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0.0f, 7.0f));
        }

        ImGui::BeginChild("##RecentProjects", ImVec2(0.0f, 0.0f), false);
        std::string projectToOpen;
        for (const auto& project : std::views::reverse(recentProjects.Projects())) {
            if (ProjectHub::Internal::RenderRecentProject(project)) {
                projectToOpen = project.path;
            }
            ImGui::Dummy(ImVec2(0.0f, ProjectHub::Internal::contentSpacing));
        }

        if (recentProjects.Projects().empty()) {
            const ImVec2 emptyMin = ImGui::GetCursorScreenPos();
            const ImVec2 emptyMax(emptyMin.x + ImGui::GetContentRegionAvail().x, emptyMin.y + 118.0f);
            ImGui::GetWindowDrawList()->AddRectFilled(emptyMin, emptyMax, ImGui::GetColorU32(ImGuiCol_FrameBg), 8.0f);
            const char* title = "No recent projects";
            const char* description = "Open an existing project or create a new one.";
            ImGui::GetWindowDrawList()->AddText(ImVec2(emptyMin.x + 16.0f, emptyMin.y + 51.0f), ImGui::GetColorU32(ImGuiCol_CheckMark), Icons::Tabler::folderOff);
            ImGui::GetWindowDrawList()->AddText(ImVec2(emptyMin.x + 46.0f, emptyMin.y + 31.0f), ImGui::GetColorU32(ImGuiCol_Text), title);
            ImGui::GetWindowDrawList()->AddText(ImVec2(emptyMin.x + 46.0f, emptyMin.y + 59.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), description);
            ImGui::Dummy(ImVec2(0.0f, 118.0f));
        }
        ImGui::EndChild();

        if (!projectToOpen.empty()) {
            OpenProject(Common::Path(projectToOpen));
        }
    }

    void ProjectHubWindow::RenderCreateProjectPopup()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(456.0f, 0.0f), ImGuiCond_Appearing);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 22.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 11.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        if (ImGui::BeginPopupModal("##CreateProjectPopup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
            const std::string createProjectLabel = Widgets::Label(Icons::Tabler::templateIcon, "Create project");
            ImGui::TextUnformatted(createProjectLabel.c_str());
            ImGui::TextDisabled("Start from a template and open it right away.");
            ImGui::Dummy(ImVec2(0.0f, 13.0f));

            ImGui::TextDisabled("NAME");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputTextWithHint("##ProjectName", "MyProject", &projectName);
            ImGui::Dummy(ImVec2(0.0f, 7.0f));

            ImGui::TextDisabled("LOCATION");
            const float browseButtonWidth = 76.0f;
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - browseButtonWidth - ImGui::GetStyle().ItemSpacing.x);
            ImGui::InputTextWithHint("##ProjectDirectory", "Choose a parent folder", &directory);
            ImGui::SameLine();
            const std::string browseLabel = Widgets::Label(Icons::Tabler::folderOpen, "Browse");
            if (ImGui::Button(browseLabel.c_str(), ImVec2(browseButtonWidth, 0.0f))) {
                if (const auto selectedDirectory = PlatformUtils::SelectDirectory("Choose Project Location", directory)) {
                    directory = *selectedDirectory;
                }
            }
            ImGui::Dummy(ImVec2(0.0f, 7.0f));

            ImGui::TextDisabled("TEMPLATE");
            const char* selectedTemplate = projectTemplates.empty()
                ? "No templates available"
                : projectTemplates[selectedTemplateIndex].name.c_str();
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##ProjectTemplate", selectedTemplate)) {
                for (int i = 0; i < static_cast<int>(projectTemplates.size()); i++) {
                    const bool selected = i == selectedTemplateIndex;
                    const std::string templateLabel = Widgets::Label(Icons::Tabler::templateIcon, projectTemplates[i].name);
                    if (ImGui::Selectable(templateLabel.c_str(), selected)) {
                        selectedTemplateIndex = i;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (!statusMessage.empty()) {
                ImGui::Dummy(ImVec2(0.0f, 9.0f));
                const std::string statusLabel = Widgets::Label(Icons::Tabler::circleX, statusMessage);
                Widgets::PushErrorTextColor();
                ImGui::TextWrapped("%s", statusLabel.c_str());
                ImGui::PopStyleColor();
            }

            ImGui::Dummy(ImVec2(0.0f, 15.0f));
            const float footerWidth = 178.0f;
            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - footerWidth);
            const std::string cancelLabel = Widgets::Label(Icons::Tabler::x, "Cancel");
            if (ImGui::Button(cancelLabel.c_str(), ImVec2(82.0f, 36.0f))) {
                statusMessage.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            const std::string confirmLabel = Widgets::Label(Icons::Tabler::check, "Create##Confirm");
            if (Widgets::PrimaryButton(confirmLabel.c_str(), ImVec2(88.0f, 36.0f))) {
                if (projectTemplates.empty()) {
                    statusMessage = "No project templates are available.";
                } else {
                    const auto result = projectService.CreateProject(CreateProjectDesc {
                        .name = projectName,
                        .parentDirectory = Common::Path(directory),
                        .templateDirectory = projectTemplates[selectedTemplateIndex].path
                    });
                    if (result.IsOk()) {
                        LogInfo(ProjectHub, "created project '{}' at '{}'", projectName, result.Value().String());
                        ImGui::CloseCurrentPopup();
                        OpenProject(result.Value());
                    } else {
                        statusMessage = result.Error();
                    }
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(4);
    }

    void ProjectHubWindow::OpenProject(const Common::Path& inProjectPath)
    {
        const auto project = ProjectService::ValidateProject(inProjectPath);
        if (project.IsErr()) {
            statusMessage = project.Error();
            return;
        }
        if (const auto result = recentProjects.Record(project.Value()); result.IsErr()) {
            LogWarning(ProjectHub, "{}", result.Error());
        }
        if (const auto result = projectLauncher.Launch(project.Value()); result.IsErr()) {
            statusMessage = result.Error();
            return;
        }
        statusMessage.clear();
        RequestClose();
    }
}
