//
// Created by johnk on 2025/8/3.
//

#include <QWebChannel>

#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/moc_ProjectHub.cpp>
#include <Core/Log.h>

namespace Editor {
    ProjectHubBridge::ProjectHubBridge(ProjectHub* parent)
        : QObject(parent)
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
