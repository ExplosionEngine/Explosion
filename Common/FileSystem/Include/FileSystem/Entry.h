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

        bool IsExists();

        std::string GetAbsolutePath();

        std::string GetRelativePath(const std::string& inputPath);

        std::string GetParent();

        bool IsDirectory();

        bool IsFile();

        void Rename(const std::string& fullName);

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
