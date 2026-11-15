#pragma once

#include <Common/Memory.h>
#include <Editor/EditorContext.h>
#include <Editor/Project/ProjectLauncher.h>
#include <Editor/Project/ProjectService.h>
#include <Editor/Project/RecentProjectStore.h>
#include <Runtime/Window.h>

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
        float NextDeltaSeconds();

        Common::UniquePtr<EditorContext> context;
        Common::UniquePtr<ProjectService> projectService;
        Common::UniquePtr<RecentProjectStore> recentProjects;
        Common::UniquePtr<ProjectLauncher> projectLauncher;
        Common::UniquePtr<Runtime::Window> mainWindow;
        double lastFrameSeconds;
    };
}
