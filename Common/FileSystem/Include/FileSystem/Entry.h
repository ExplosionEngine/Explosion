//
// Created by Mevol on 2021/8/8.
//

#ifndef EXPLOSION_ENTRY_H
#define EXPLOSION_ENTRY_H

#include <iostream>
#include <string>
#include <vector>
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
        }

        Entry<T>& operator=(const Entry<T>& entry)
        {
            path = entry.path;
            return *this;
        }

    public:

        const T& cast() const
        {
            return static_cast<const T&>(*this);
        }

        [[nodiscard]] bool IsExists() const
        {
            return exists(path);
        }

        [[nodiscard]] std::string GetAbsolutePath() const
        {
            return absolute(path).string();
        }

        static void SplitStr(const std::string& str, const char symbol, std::vector<std::string>& list)
        {
            if (str.empty()) {
                return ;
            }
            list.clear();
            int startIndex = 0;
            for (int i = 0; i < str.size(); ++i) {
                if (str.at(i) == symbol && (i-startIndex) > 0) {
                    list.push_back(str.substr(startIndex,i-startIndex));
                    startIndex = i+1;
                }
                if (i == str.size()-1 && (i-startIndex) > 0) {
                    list.push_back(str.substr(startIndex,i-startIndex+1));
                }
            }
        }

        [[nodiscard]] std::string GetRelativePath(const std::string& inputPath) const
        {
            std::string curPath = GetAbsolutePath();
            fs::path tmpPath(inputPath);
            std::string inPath = absolute(tmpPath).string();
            std::vector<std::string> curPathList = {};
            std::vector<std::string> inPathList = {};
#ifdef _WIN32
            SplitStr(curPath,'\\',curPathList);
            SplitStr(inPath,'\\',inPathList);
#else
            SplitStr(curPath,'/',curPathList);
            SplitStr(inPath,'/',inPathList);
#endif
            int samePos = -1;
            std::string result;

            for (int i = 0; i < std::min(inPathList.size(),curPathList.size()); ++i) {
                if (curPathList.at(i) == inPathList.at(i)) {
                    samePos = i;
                }
                else {
                    break;
                }
            }
            for (int i = 0; i < inPathList.size()-samePos-1; ++i) {
#ifdef _WIN32
                result += "..\\";
#else
                result += "../";
#endif
            }
            for (int i = 0; i < curPathList.size()-samePos-1; ++i) {
                result += curPathList.at(samePos+1+i);
#ifdef _WIN32
                if (i != curPathList.size()-samePos-2) {
                    result += "\\";
                }
#else
                if (i != curPathList.size()-samePos-2) {
                    result += "/";
                }
#endif
            }
            return result;
        }

        [[nodiscard]] std::string GetParent() const
        {
            if (IsExists()) {
                return path.parent_path().string();
            }
            throw std::runtime_error("Path not exist!");
        }

        [[nodiscard]] bool IsDirectory() const
        {
            return IsExists() && fs::is_directory(path);
        }

        [[nodiscard]] bool IsFile() const
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
