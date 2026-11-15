#include <Common/Time.h>
#include <Core/Paths.h>
#include <Editor/EditorApplication.h>
#include <Editor/EditorLog.h>
#include <Editor/EditorMainWindow.h>
#include <Editor/ProjectHubWindow.h>
#include <Runtime/Engine.h>

namespace Editor {
    EditorApplication::EditorApplication(EditorApplicationDesc inDesc)
        : lastFrameSeconds(Common::TimePoint::Now().ToSeconds())
    {
        if (inDesc.mode == EditorApplicationMode::editor) {
            context = Common::MakeUnique<EditorContext>();
            context->OpenProjectLevel();
            mainWindow.Reset(new EditorMainWindow(*context, EditorLogStream::Get(), Core::Paths::GameAssetDir()));
        } else {
            projectService = Common::MakeUnique<ProjectService>(Core::Paths::EngineResDir() / "Editor" / "ProjectTemplates");
            recentProjects = Common::MakeUnique<RecentProjectStore>(Core::Paths::EngineCacheDir() / "Editor" / "ProjectHub" / "RecentProjects.json");
            projectLauncher = Common::MakeUnique<ProjectLauncher>(EditorLaunchOptions {
                .executablePath = Core::Paths::ExecutablePath().String(),
                .rhiType = inDesc.rhiType,
                .gpuDebug = inDesc.gpuDebug,
                .softwareGpu = inDesc.softwareGpu
            });
            mainWindow.Reset(new ProjectHubWindow(*projectService, *recentProjects, *projectLauncher));
        }
    }

    EditorApplication::~EditorApplication() = default;

    int EditorApplication::Run()
    {
        while (!mainWindow->ShouldClose()) {
            const float deltaSeconds = NextDeltaSeconds();
            Runtime::EngineHolder::Get().Tick(deltaSeconds);
        }
        return 0;
    }

    float EditorApplication::NextDeltaSeconds()
    {
        const double now = Common::TimePoint::Now().ToSeconds();
        const float deltaSeconds = static_cast<float>(now - lastFrameSeconds);
        lastFrameSeconds = now;
        return deltaSeconds;
    }
}
