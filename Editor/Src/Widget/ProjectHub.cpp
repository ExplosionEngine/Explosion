//
// Created by johnk on 2025/8/3.
//

#include <format>

#include <QFileDialog>

#include <Editor/Widget/ProjectHub.h>
#include <Editor/Widget/moc_ProjectHub.cpp>
#include <Core/Log.h>
#include <Core/EngineVersion.h>
#include <Core/Paths.h>
#include <Mirror/Mirror.h>
#include <Common/Result.h>
#include <Common/Serialization.h>
#include <Common/Template.h>
#include <Editor/Qt/JsonSerialization.h>

namespace Editor::Internal {
    constexpr std::string_view templateFileExtension = ".tpl";
    constexpr std::string_view cmakeMinVersion = "3.25";

    static Common::Path StripTemplateExtension(const Common::Path& inRelativePath)
    {
        const std::string str = inRelativePath.String();
        return { str.substr(0, str.size() - templateFileExtension.size()) };
    }

    static QJsonValue ToJsonValue(const CreateProjectResult& inResult)
    {
        QJsonValue value;
        QtJsonSerialize(value, inResult);
        return value;
    }

    static Common::Result<void, std::string> RenderProjectTemplate(
        const Common::Path& inTemplateDir, const Common::Path& inProjectDir, const std::string& inProjectName)
    {
        Common::TemplateEngine templateEngine;
        templateEngine
            .Set("projectName", inProjectName)
            .Set("cmakeMinVersion", std::string(cmakeMinVersion));

        Common::Result<void, std::string> result = Common::Ok();
        inTemplateDir.TraverseRecurse([&](const Common::Path& inPath) -> bool {
            if (inPath.IsDirectory()) {
                return true;
            }

            const Common::Path relativePath = inPath.Relative(inTemplateDir);
            const std::string extension = inPath.Extension();
            const bool isTemplate = std::string_view(extension) == templateFileExtension;
            const Common::Path dstPath = inProjectDir / (isTemplate ? StripTemplateExtension(relativePath) : relativePath);
            dstPath.Parent().MakeDir();

            if (!isTemplate) {
                inPath.CopyTo(dstPath);
                return true;
            }
            if (auto renderResult = templateEngine.RenderFileTo(inPath.String(), dstPath.String());
                renderResult.IsErr()) {
                result = Common::Err(renderResult.Error());
                return false;
            }
            return true;
        });
        return result;
    }
}

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

    QJsonValue ProjectHubBackend::CreateProject(const QString& inName, const QString& inDirectory, const QString& inTemplatePath)
    {
        const std::string name = inName.toStdString();
        const std::string directory = inDirectory.toStdString();
        const std::string templatePath = inTemplatePath.toStdString();

        if (name.empty() || directory.empty() || templatePath.empty()) {
            return Internal::ToJsonValue({ .success = false, .error = "Project name, directory and template must not be empty.", .projectPath = {} });
        }

        const Common::Path templateDir(templatePath);
        if (!templateDir.Exists() || !templateDir.IsDirectory()) {
            return Internal::ToJsonValue({ .success = false, .error = std::format("Project template '{}' does not exist.", templatePath), .projectPath = {} });
        }

        const Common::Path projectDir = Common::Path(directory) / name;
        if (projectDir.Exists()) {
            return Internal::ToJsonValue({ .success = false, .error = std::format("Target directory '{}' already exists.", projectDir.String()), .projectPath = {} });
        }

        if (const auto result = Internal::RenderProjectTemplate(templateDir, projectDir, name);
            result.IsErr()) {
            return Internal::ToJsonValue({ .success = false, .error = result.Error(), .projectPath = {} });
        }

        recentProjects.emplace_back(RecentProjectInfo { name, projectDir.String() });
        Common::JsonSerializeToFile(recentProjectsFile.String(), recentProjects);
        emit RecentProjectsChanged();

        LogInfo(ProjectHub, "created project '{}' at '{}'", name, projectDir.String());
        return Internal::ToJsonValue({ .success = true, .error = {}, .projectPath = projectDir.String() });
    }

    void ProjectHubBackend::OpenProject(const QString& inProjectPath) // NOLINT
    {
        // TODO: launch the editor for the project at inProjectPath
        LogInfo(ProjectHub, "open project '{}'", inProjectPath.toStdString());
    }

    QString ProjectHubBackend::BrowseDirectory() const // NOLINT
    {
        return QFileDialog::getExistingDirectory(nullptr, "Select Project Directory", QDir::rootPath());
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
