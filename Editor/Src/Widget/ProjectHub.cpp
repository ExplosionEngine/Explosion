//
// Created by johnk on 2025/8/3.
//

#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/moc_ProjectHub.cpp>
#include <Core/Log.h>
#include <Core/EngineVersion.h>

namespace Editor {
    ProjectHubBridge::ProjectHubBridge(ProjectHub* parent)
        : QObject(parent)
        , engineVersion(QString::fromStdString(std::format("v{}.{}.{}", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH)))
    {
    }

    void ProjectHubBridge::CreateProject() const
    {
        // TODO
        LogInfo(ProjectHub, "ProjectHubBridge::CreateProject");
    }

    ProjectHub::ProjectHub(QWidget* inParent)
        : WebWidget(inParent)
    {
        setFixedSize(800, 600);
        Load("/project-hub");

        bridge = new ProjectHubBridge(this);
        GetWebChannel()->registerObject("bridge", bridge);
    }
} // namespace Editor
