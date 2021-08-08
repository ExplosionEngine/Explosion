//
// Created by Mevol on 2021/8/8.
//

#ifndef EXPLOSION_ENTRY_H
#define EXPLOSION_ENTRY_H

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace Explosion::FileSystem {
    template <typename T>
    class Entry {
    public:
        explicit Entry(std::string path);
        virtual ~Entry();
        Entry(const Entry<T>& entry);
        Entry<T> &operator=(const Entry<T> &entry);

    public:
        const T& cast() const;
        [[nodiscard]] bool IsExists() const;
        std::string GetAbsolutePath();
        std::string GetRelativePath(std::string inputPath);
        Entry<T> GetParent();
        bool IsDirectory();
        [[nodiscard]] bool IsFile() const;
        void Rename(const std::string& fullName);
        void Make();

    protected:
        friend T;
        fs::path path;
    };
}

#endif //EXPLOSION_ENTRY_H
