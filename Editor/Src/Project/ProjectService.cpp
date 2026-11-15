#include <algorithm>
#include <filesystem>
#include <format>
#include <string_view>

#include <Common/Template.h>
#include <Editor/Project/ProjectService.h>

namespace Editor::Project::Internal {
    constexpr std::string_view templateFileExtension = ".tpl";
    constexpr std::string_view cmakeMinVersion = "3.25";

    static bool IsValidProjectName(const std::string& inName)
    {
        return !inName.empty() && std::ranges::all_of(inName, [](unsigned char inCharacter) -> bool {
            return (inCharacter >= 'a' && inCharacter <= 'z') || (inCharacter >= 'A' && inCharacter <= 'Z')
                || (inCharacter >= '0' && inCharacter <= '9') || inCharacter == '_' || inCharacter == '-';
        });
    }

    static Common::Result<void, std::string> RenderProjectTemplate(const Common::Path& inTemplateDirectory, const Common::Path& inProjectDirectory, const std::string& inName)
    {
        Common::TemplateEngine templateEngine;
        templateEngine.Set("projectName", inName).Set("cmakeMinVersion", std::string(cmakeMinVersion));
        const std::filesystem::path templateDirectory(inTemplateDirectory.String());
        const std::filesystem::path projectDirectory(inProjectDirectory.String());
        for (const auto& entry : std::filesystem::recursive_directory_iterator(templateDirectory)) {
            if (entry.is_symlink()) {
                return Common::Err(std::string("Project templates cannot contain symbolic links."));
            }
            const auto relativePath = entry.path().lexically_relative(templateDirectory);
            auto destination = projectDirectory / relativePath;
            if (entry.is_directory()) {
                std::filesystem::create_directories(destination);
                continue;
            }
            std::filesystem::create_directories(destination.parent_path());
            if (entry.path().extension() == templateFileExtension) {
                destination.replace_extension();
                if (auto result = templateEngine.RenderFileTo(entry.path().string(), destination.string()); result.IsErr()) {
                    return result;
                }
            } else {
                std::filesystem::copy_file(entry.path(), destination);
            }
        }
        return Common::Ok();
    }
}

namespace Editor {
    ProjectService::ProjectService(Common::Path inTemplatesDirectory)
        : templatesDirectory(std::move(inTemplatesDirectory))
    {
    }

    Common::Result<std::vector<ProjectTemplateInfo>, std::string> ProjectService::ListTemplates() const
    {
        try {
            std::vector<ProjectTemplateInfo> result;
            for (const auto& entry : std::filesystem::directory_iterator(templatesDirectory.String())) {
                if (entry.is_directory() && std::filesystem::is_regular_file(entry.path() / "CMakeLists.txt.tpl")
                    && std::filesystem::is_directory(entry.path() / "Game")) {
                    result.emplace_back(ProjectTemplateInfo { entry.path().filename().string(), Common::Path(entry.path()) });
                }
            }
            std::ranges::sort(result, {}, &ProjectTemplateInfo::name);
            return Common::Ok(std::move(result));
        } catch (const std::filesystem::filesystem_error& error) {
            return Common::Err(std::format("Could not list project templates: {}", error.what()));
        }
    }

    Common::Result<Common::Path, std::string> ProjectService::CreateProject(const CreateProjectDesc& inDesc) const
    {
        if (!Project::Internal::IsValidProjectName(inDesc.name) || inDesc.parentDirectory.Empty() || inDesc.templateDirectory.Empty()) {
            return Common::Err(std::string("Choose a location and template. Project names may contain letters, digits, '_' and '-'."));
        }

        std::filesystem::path projectDirectory;
        bool created = false;
        auto cleanup = [&]() -> void {
            if (created) {
                std::error_code error;
                std::filesystem::remove_all(projectDirectory, error);
            }
        };
        try {
            const std::filesystem::path templateDirectory(inDesc.templateDirectory.String());
            if (!std::filesystem::is_regular_file(templateDirectory / "CMakeLists.txt.tpl") || !std::filesystem::is_directory(templateDirectory / "Game")) {
                return Common::Err(std::string("The selected template must contain CMakeLists.txt.tpl and a Game directory."));
            }
            projectDirectory = std::filesystem::absolute(std::filesystem::path(inDesc.parentDirectory.String()) / inDesc.name).lexically_normal();
            std::filesystem::create_directories(projectDirectory.parent_path());
            created = std::filesystem::create_directory(projectDirectory);
            if (!created) {
                return Common::Err(std::format("Target directory '{}' already exists.", projectDirectory.string()));
            }
            if (auto result = Project::Internal::RenderProjectTemplate(inDesc.templateDirectory, Common::Path(projectDirectory), inDesc.name); result.IsErr()) {
                cleanup();
                return Common::Err(result.Error());
            }
            return Common::Ok(Common::Path(projectDirectory));
        } catch (const std::filesystem::filesystem_error& error) {
            cleanup();
            return Common::Err(std::format("Could not create project: {}", error.what()));
        }
    }

    Common::Result<Common::Path, std::string> ProjectService::ValidateProject(const Common::Path& inPath)
    {
        try {
            const std::filesystem::path path(inPath.String());
            if (!std::filesystem::is_regular_file(path / "CMakeLists.txt") || !std::filesystem::is_directory(path / "Game")) {
                return Common::Err(std::string("Choose an Explosion project containing CMakeLists.txt and a Game directory."));
            }
            return Common::Ok(Common::Path(std::filesystem::canonical(path)));
        } catch (const std::filesystem::filesystem_error& error) {
            return Common::Err(std::format("Could not open project: {}", error.what()));
        }
    }
}
