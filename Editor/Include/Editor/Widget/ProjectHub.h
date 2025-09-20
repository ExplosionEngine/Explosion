//
// Created by johnk on 2025/8/3.
//

#pragma once

#include <Editor/Widget/WebWidget.h>
#include <Mirror/Meta.h>
#include <Common/FileSystem.h>
#include <Editor/Qt/EngineSerialization.h>

namespace Editor {
    class ProjectHub;

    struct EClass() RecentProjectInfo {
        EClassBody(RecentProjectInfo)

        EProperty() std::string name;
        EProperty() std::string path;
    };

    struct EClass() ProjectTemplateInfo {
        EClassBody(ProjectTemplateInfo)

        EProperty() std::string name;
        EProperty() std::string path;
    };

    class ProjectHubBackend final : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString engineVersion READ GetEngineVersion CONSTANT)
        Q_PROPERTY(QJsonValue projectTemplates READ GetProjectTemplates CONSTANT)
        Q_PROPERTY(QJsonValue recentProjects READ GetRecentProjects)

    public:
        explicit ProjectHubBackend(ProjectHub* parent = nullptr);
        ~ProjectHubBackend() override;

    public Q_SLOTS:
        void CreateProject() const;

    private:
        QString GetEngineVersion() const;
        QJsonValue GetProjectTemplates() const;
        QJsonValue GetRecentProjects() const;

        Common::Path recentProjectsFile;
        std::string engineVersion;
        std::vector<ProjectTemplateInfo> projectTemplates;
        std::vector<RecentProjectInfo> recentProjects;
    };

    class ProjectHub final : public WebWidget {
        Q_OBJECT

    public:
        explicit ProjectHub(QWidget* inParent = nullptr);

    private:
        ProjectHubBackend* backend;
    };
}
