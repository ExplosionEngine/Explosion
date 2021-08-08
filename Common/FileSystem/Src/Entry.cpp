//
// Created by John Kindem on 2021/8/8.
//

#include <FileSystem/Entry.h>

namespace Explosion::FileSystem {
    template<typename T>
    Entry<T>::Entry(std::string path) : path(std::move(path)) {}

    template<typename T>
    Entry<T>::~Entry() = default;

    template<typename T>
    Entry<T>::Entry(const Entry<T>& entry)
    {
        path = entry.path;
        return *this;
    }

    template<typename T>
    Entry<T>& Entry<T>::operator=(const Entry<T>& entry)
    {
        path = entry.path;
        return *this;
    }

    template<typename T>
    const T& Entry<T>::cast() const
    {
        return static_cast<const T&>(*this);
    }

    template<typename T>
    bool Entry<T>::IsExists() const
    {
        return exists(path);
    }

    template<typename T>
    std::string Entry<T>::GetAbsolutePath()
    {
        if (IsExists()) {
            return absolute(path).string();
        }
        throw std::runtime_error("Path not exist!");
    }

    template<typename T>
    std::string Entry<T>::GetRelativePath(std::string inputPath)
    {
        if (IsExists()) {
            return path.relative_path().string();
        }
        throw std::runtime_error("Path not exist!");
    }

    template<typename T>
    Entry<T> Entry<T>::GetParent()
    {
        if (IsExists()) {
            return Entry<T>(path.parent_path().string);
        }
        throw std::runtime_error("Path not exist!");
    }

    template<typename T>
    bool Entry<T>::IsDirectory()
    {
        return IsExists() && fs::is_directory(path);
    }

    template<typename T>
    bool Entry<T>::IsFile() const
    {
        return IsExists() && fs::is_regular_file(path);
    }

    template<typename T>
    void Entry<T>::Rename(const std::string& fullName)
    {
        if (IsExists()) {
            rename(GetAbsolutePath().c_str(),fullName.c_str());
        }
        throw std::runtime_error("Path not exist!");
    }

    template<typename T>
    void Entry<T>::Make()
    {
        static_cast<T*>(this)->MakeImpl();
    }
}
