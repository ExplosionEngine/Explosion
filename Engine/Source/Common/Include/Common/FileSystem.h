//
// Created by johnk on 2025/1/2.
//

#pragma once

#include <filesystem>

namespace Common {
    class Path {
    public:
        using TraverseFunc = std::function<bool(const Path&)>;

        static Path WorkingDirectory();

        Path();
        Path(const std::filesystem::path& inPath, bool inNeedFixedUp = true); // NOLINT
        Path(const std::string& inPath, bool inNeedFixedUp = true); // NOLINT
        Path(const char* inPath, bool inNeedFixedUp = true); // NOLINT

        std::string String() const;
        Path Parent() const;
        std::string FileName() const;
        std::string FileNameWithoutExtension() const;
        bool Empty() const;
        bool Exists() const;
        bool IsFile() const;
        bool IsDirectory() const;
        bool HasExtension() const;
        std::string Extension() const;
        bool IsAbsolute() const;
        bool IsRelative() const;
        Path Absolute() const;
        Path Relative(const Path& inRelative) const;
        Path Canonical() const;
        size_t Traverse(const TraverseFunc& inFunc) const;
        size_t TraverseRecurse(const TraverseFunc& inFunc) const;
        void CopyTo(const Path& inPath) const;

        Path operator/(const Path& inPath) const;
        Path operator/(const std::string& inPath) const;
        Path operator/(const char* inPath) const;
        Path operator+(const Path& inPath) const;
        Path operator+(const std::string& inPath) const;
        Path operator+(const char* inPath) const;

    private:
        std::filesystem::path path;
    };
}
