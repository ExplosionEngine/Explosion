//
// Created by johnk on 2022/7/25.
//

#pragma once

#include <unordered_map>
#include <utility>
#include <filesystem>

#include <Common/Utility.h>
#include <Common/String.h>

namespace Common {
    class PathMapper {
    public:
        static PathMapper From(const std::unordered_map<std::string, std::string>& map)
        {
            std::vector<std::pair<std::string, std::string>> result;
            result.reserve(map.size());

            for (const auto& iter : map) {
                result.emplace_back(std::make_pair(iter.first, iter.second));
            }
            std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) -> bool { return a.second.length() < b.second.length(); });
            return PathMapper(std::move(result));
        }

        PathMapper() = default;
        ~PathMapper() = default;
        PathMapper(PathMapper&& other) noexcept : mappers(std::move(other.mappers)) {}
        NON_COPYABLE(PathMapper)

        [[nodiscard]] std::string Map(const std::string& path) const
        {
            for (const auto& mapper : mappers) {
                if (path.starts_with(mapper.first)) {
                    return Common::StringUtils::Replace(path, mapper.first, mapper.second);
                }
            }
            return path;
        }

    private:
        explicit PathMapper(std::vector<std::pair<std::string, std::string>> inMappers) : mappers(std::move(inMappers)) {}

        std::vector<std::pair<std::string, std::string>> mappers;
    };

    class PathUtils {
    public:
        static inline std::string GetParentPath(const std::string& absolutePath)
        {
            std::filesystem::path path(absolutePath);
            return Common::StringUtils::Replace(path.parent_path().string(), "\\", "/");
        }
    };
}
