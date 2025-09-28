//
// Created by johnk on 2025/8/3.
//

#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/moc_ProjectHub.cpp>
#include <Core/Log.h>
#include <Core/EngineVersion.h>
#include <Core/Paths.h>
#include <Mirror/Mirror.h>
#include <Editor/Qt/JsonSerialization.h>

namespace Editor {
    ProjectHubBackend::ProjectHubBackend(ProjectHub* parent)
        : QObject(parent)
        , recentProjectsFile(Core::Paths::EngineCacheDir() / "Editor" / "ProjectHub" / "RecentProjects.json")
        , engineVersion(std::format("v{}.{}.{}", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH))
    {
        const Common::Path projectTemplatesRoot = Core::Paths::EngineResDir() / "Editor" / "ProjectTemplates";
        (void) projectTemplatesRoot.Traverse([this](const Common::Path& inPath) -> bool {
            if (inPath.IsFile()) {
                return true;
            }
            projectTemplates.emplace_back(ProjectTemplateInfo { inPath.DirName(), inPath.String() });
            return true;
        });

        if (recentProjectsFile.Exists()) {
            Common::JsonDeserializeFromFile(recentProjectsFile.String(), recentProjects);
        }
    }

    ProjectHubBackend::~ProjectHubBackend()
    {
        Common::JsonSerializeToFile(recentProjectsFile.String(), recentProjects);
    }

    void ProjectHubBackend::CreateProject() const
    {
        // TODO
        LogInfo(ProjectHub, "ProjectHubBridge::CreateProject");
    }

    QString ProjectHubBackend::GetEngineVersion() const
    {
        return QString::fromStdString(engineVersion);
    }

    QJsonValue ProjectHubBackend::GetProjectTemplates() const
    {
        QJsonValue value;
        QtJsonSerialize(value, projectTemplates);
        return value;
    }

    QJsonValue ProjectHubBackend::GetRecentProjects() const
    {
        QJsonValue value;
        QtJsonSerialize(value, recentProjects);
        return value;
    }

    ProjectHub::ProjectHub(QWidget* inParent)
        : WebWidget(inParent)
    {
        setFixedSize(800, 600);
        Load("/project-hub");

        backend = new ProjectHubBackend(this);
        GetWebChannel()->registerObject("backend", backend);
    }
} // namespace Editor
