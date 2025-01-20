//
// Created by johnk on 2025/1/2.
//

#include <Common/FileSystem.h>
#include <Common/String.h>

namespace Common::Internal {
    std::filesystem::path GetUnixStylePath(const std::string& inPath)
    {
        return Common::StringUtils::Replace(std::filesystem::weakly_canonical(inPath).string(), "\\", "/");
    }

    std::filesystem::path GetUnixStylePath(const std::filesystem::path& inPath)
    {
        return GetUnixStylePath(inPath.string());
    }
}

namespace Common {
    Path Path::WorkingDirectory()
    {
        return { std::filesystem::current_path() };
    }

    Path::Path() = default;

    Path::Path(const std::filesystem::path& inPath, bool inNeedFixedUp)
        : path(inNeedFixedUp ? Internal::GetUnixStylePath(inPath) : inPath)
    {
    }

    Path::Path(const std::string& inPath, bool inNeedFixedUp)
        : path(inNeedFixedUp ? Internal::GetUnixStylePath(inPath) : std::filesystem::path(inPath))
    {
    }

    Path::Path(const char* inPath, bool inNeedFixedUp)
        : path(inNeedFixedUp ? Internal::GetUnixStylePath(std::string(inPath)) : std::filesystem::path(inPath))
    {
    }

    std::string Path::String() const
    {
        return path.string();
    }

    Path Path::operator/(const Path& inPath) const
    {
        return { String() + "/" + inPath.String(), false };
    }

    Path Path::operator/(const std::string& inPath) const
    {
        return { String() + "/" + inPath, false };
    }

    Path Path::operator/(const char* inPath) const
    {
        return { String() + "/" + inPath, false };
    }

    Path Path::operator+(const Path& inPath) const
    {
        return { String() + inPath.String(), false };
    }

    Path Path::operator+(const std::string& inPath) const
    {
        return { String() + inPath, false };
    }

    Path Path::operator+(const char* inPath) const
    {
        return { String() + inPath, false };
    }

    Path Path::Parent() const
    {
        return { path.parent_path(), false };
    }

    std::string Path::FileName() const
    {
        Assert(IsFile());
        return { path.filename().string() };
    }

    std::string Path::FileNameWithoutExtension() const
    {
        const auto extension = Extension();
        return extension.empty() ? FileName() : Common::StringUtils::Replace(FileName(), Extension(), "");
    }

    bool Path::Empty() const
    {
        return path.empty();
    }

    bool Path::Exists() const
    {
        return std::filesystem::exists(path);
    }

    bool Path::IsFile() const
    {
        return !std::filesystem::is_directory(path);
    }

    bool Path::IsDirectory() const
    {
        return std::filesystem::is_directory(path);
    }

    bool Path::HasExtension() const
    {
        return path.has_extension();
    }

    std::string Path::Extension() const
    {
        Assert(IsFile());
        return path.extension().string();
    }

    bool Path::IsAbsolute() const
    {
        return path.is_absolute();
    }

    bool Path::IsRelative() const
    {
        return path.is_relative();
    }

    Path Path::Absolute() const
    {
        return { std::filesystem::absolute(path) };
    }

    Path Path::Relative(const Path& inRelative) const
    {
        return { std::filesystem::relative(path, inRelative.path) };
    }

    Path Path::Canonical() const
    {
        return { std::filesystem::weakly_canonical(path) };
    }

    size_t Path::Traverse(const TraverseFunc& inFunc) const
    {
        Assert(IsDirectory());
        size_t count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            count++;
            if (!inFunc(entry.path())) {
                break;
            }
        }
        return count;
    }

    size_t Path::TraverseRecurse(const TraverseFunc& inFunc) const
    {
        Assert(IsDirectory());
        size_t count = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            count++;
            if (!inFunc(entry.path())) {
                break;
            }
        }
        return count;
    }

    void Path::CopyTo(const Path& inPath) const
    {
        std::filesystem::copy(path, inPath.path, std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive);
    }

    void Path::MakeDir() const
    {
        std::filesystem::create_directories(path);
    }
} // namespace Common
