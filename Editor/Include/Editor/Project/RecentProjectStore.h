#pragma once

#include <string>
#include <vector>

#include <Common/FileSystem.h>
#include <Common/Result.h>

namespace Editor {
    struct RecentProjectInfo {
        std::string name;
        std::string path;
    };

    class RecentProjectStore final {
    public:
        explicit RecentProjectStore(Common::Path inFile);

        Common::Result<void, std::string> Load();
        Common::Result<void, std::string> Record(const Common::Path& inProjectPath);
        const std::vector<RecentProjectInfo>& Projects() const;

    private:
        Common::Path file;
        std::vector<RecentProjectInfo> projects;
    };
}
