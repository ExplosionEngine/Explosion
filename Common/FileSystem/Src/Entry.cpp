//
// Created by Mevol on 2021/8/8.
//

#include <FileSystem/Entry.h>

namespace Explosion::FileSystem {

    template<typename T>
    bool Entry<T>::IsExists()
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
    std::string Entry<T>::GetRelativePath(const std::string& inputPath)
    {
        if (IsExists()) {
            return path.relative_path().string();
        }
        throw std::runtime_error("Path not exist!");
    }

    template<typename T>
    bool Entry<T>::IsDirectory()
    {
        return IsExists() && fs::is_directory(path);
    }

    template<typename T>
    bool Entry<T>::IsFile()
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
    std::string Entry<T>::GetParent()
    {
        if (IsExists()) {
            return path.parent_path().string();
        }
        throw std::runtime_error("Path not exist!");
    }



}
