//
// Created by johnk on 2025/1/2.
//

#pragma once

#include <filesystem>

#include <Common/String.h>

namespace Common {
    class FileSystem {
    public:
        static std::string GetUnixStylePath(const std::string& inPath);
        static std::string GetUnixStylePath(const std::filesystem::path& inPath);
    };
}
