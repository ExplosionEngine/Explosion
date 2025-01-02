//
// Created by johnk on 2025/1/2.
//

#include <Common/FileSystem.h>

namespace Common {
    std::string FileSystem::GetUnixStylePath(const std::string& inPath)
    {
        return Common::StringUtils::Replace(std::filesystem::weakly_canonical(inPath).string(), "\\", "/");
    }

    std::string FileSystem::GetUnixStylePath(const std::filesystem::path& inPath)
    {
        return GetUnixStylePath(inPath.string());
    }
}
