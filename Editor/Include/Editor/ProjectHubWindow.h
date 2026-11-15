#pragma once

#include <string>
#include <vector>

#include <Editor/ImGui/ImGuiWindow.h>
#include <Editor/Project/ProjectLauncher.h>
#include <Editor/Project/ProjectService.h>
#include <Editor/Project/RecentProjectStore.h>

namespace Editor {
    class ProjectHubWindow final : public ImGuiWindow {
    public:
        ProjectHubWindow(ProjectService& inProjectService, RecentProjectStore& inRecentProjects, ProjectLauncher& inProjectLauncher);
        ~ProjectHubWindow() override;

    private:
        void RenderContents() override;
        void RenderActionBar();
        void RenderRecentProjects();
        void RenderCreateProjectPopup();
        void OpenProject(const Common::Path& inProjectPath);

        ProjectService& projectService;
        RecentProjectStore& recentProjects;
        ProjectLauncher& projectLauncher;
        std::vector<ProjectTemplateInfo> projectTemplates;
        std::string projectName;
        std::string directory;
        int selectedTemplateIndex;
        std::string statusMessage;
    };
}
