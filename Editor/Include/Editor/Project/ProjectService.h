#pragma once

#include <string>
#include <vector>

#include <Common/FileSystem.h>
#include <Common/Result.h>

namespace Editor {
    struct ProjectTemplateInfo {
        std::string name;
        Common::Path path;
    };

    struct CreateProjectDesc {
        std::string name;
        Common::Path parentDirectory;
        Common::Path templateDirectory;
    };

    class ProjectService final {
    public:
        explicit ProjectService(Common::Path inTemplatesDirectory);

        Common::Result<std::vector<ProjectTemplateInfo>, std::string> ListTemplates() const;
        Common::Result<Common::Path, std::string> CreateProject(const CreateProjectDesc& inDesc) const;
        static Common::Result<Common::Path, std::string> ValidateProject(const Common::Path& inPath);

    private:
        Common::Path templatesDirectory;
    };
}
