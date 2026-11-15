#include <algorithm>
#include <filesystem>
#include <format>

#include <Common/File.h>
#include <Editor/Project/RecentProjectStore.h>

namespace Editor {
    RecentProjectStore::RecentProjectStore(Common::Path inFile)
        : file(std::move(inFile))
    {
    }

    Common::Result<void, std::string> RecentProjectStore::Load()
    {
        try {
            if (!file.Exists()) {
                projects.clear();
                return Common::Ok();
            }
            auto result = Common::FileUtils::ReadJsonFile(file.String());
            if (result.IsErr()) {
                return Common::Err(result.Error());
            }
            const auto& document = result.Value();
            if (!document.IsArray()) {
                return Common::Err(std::string("Recent projects must be a JSON array."));
            }
            std::vector<RecentProjectInfo> loaded;
            for (const auto& entry : document.GetArray()) {
                if (!entry.IsObject() || !entry.HasMember("path") || !entry["path"].IsString()
                    || (entry.HasMember("name") && !entry["name"].IsString())) {
                    return Common::Err(std::string("Invalid recent project entry."));
                }
                const std::string path = entry["path"].GetString();
                loaded.emplace_back(RecentProjectInfo { entry.HasMember("name") ? entry["name"].GetString() : Common::Path(path).DirName(), path });
            }
            projects = std::move(loaded);
            return Common::Ok();
        } catch (const std::filesystem::filesystem_error& error) {
            return Common::Err(std::format("Could not load recent projects: {}", error.what()));
        }
    }

    Common::Result<void, std::string> RecentProjectStore::Record(const Common::Path& inProjectPath)
    {
        try {
            const Common::Path projectPath = inProjectPath.Canonical();
            std::vector<RecentProjectInfo> updated = projects;
            std::string name = projectPath.DirName();
            std::erase_if(updated, [&](const RecentProjectInfo& inProject) -> bool {
                std::error_code error;
                const bool same = inProject.path == projectPath.String() || std::filesystem::equivalent(inProject.path, projectPath.String(), error);
                if (same) {
                    name = inProject.name;
                }
                return same;
            });
            updated.emplace_back(RecentProjectInfo { name, projectPath.String() });

            rapidjson::Document document;
            document.SetArray();
            auto& allocator = document.GetAllocator();
            for (const auto& project : updated) {
                rapidjson::Value entry(rapidjson::kObjectType);
                entry.AddMember("name", rapidjson::Value(project.name.c_str(), allocator), allocator);
                entry.AddMember("path", rapidjson::Value(project.path.c_str(), allocator), allocator);
                document.PushBack(entry, allocator);
            }
            if (auto result = Common::FileUtils::WriteJsonFile(file.String(), document); result.IsErr()) {
                return result;
            }
            projects = std::move(updated);
            return Common::Ok();
        } catch (const std::filesystem::filesystem_error& error) {
            return Common::Err(std::format("Could not save recent projects: {}", error.what()));
        }
    }

    const std::vector<RecentProjectInfo>& RecentProjectStore::Projects() const
    {
        return projects;
    }
}
