//
// Created by Mevol on 2021/8/8.
//

#ifndef EXPLOSION_ENTRY_H
#define EXPLOSION_ENTRY_H

#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace Explosion::FileSystem {
    template <typename T>
    class Entry {
    public:
        explicit Entry(const std::string& inPath)
        {
            fs::path input(inPath);
            path = absolute(input);
        }

        virtual ~Entry() = default;

        Entry(const Entry<T>& entry)
        {
            path = entry.path;
            return *this;
        }

        Entry<T>& operator=(const Entry<T>& entry)
        {
            path = entry.path;
            return *this;
        }

    public:

        const T& cast()
        {
            return static_cast<const T&>(*this);
        }

        [[nodiscard]] bool IsExists()
        {
            return exists(path);
        }

        std::string GetAbsolutePath()
        {
            return absolute(path).string();
        }

        std::string GetRelativePath(const std::string& inputPath)
        {
            return path.relative_path().string();
        }

        std::string GetParent()
        {
            if (IsExists()) {
                return path.parent_path().string();
            }
            throw std::runtime_error("Path not exist!");
        }

        bool IsDirectory()
        {
            return IsExists() && fs::is_directory(path);
        }

        [[nodiscard]] bool IsFile()
        {
            return IsExists() && fs::is_regular_file(path);
        }

        void Rename(const std::string& fullName)
        {
            if (IsExists()) {
                fs::path renamePath(fullName);
                if (exists(renamePath)) {
                    std::cout << "Warning: rename destination: '" << fullName << "' already exist!" << std::endl;
                    return ;
                }
                rename(GetAbsolutePath().c_str(),fullName.c_str());
                return ;
            }
            throw std::runtime_error("Path not exist!");
        }

        void Make()
        {
            static_cast<T*>(this)->MakeImpl();
        }

    protected:
        friend T;
        fs::path path;
    };
}

#endif //EXPLOSION_ENTRY_H
