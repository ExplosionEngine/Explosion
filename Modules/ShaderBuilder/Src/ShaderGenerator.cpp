//
// Created by Zach Lee on 2021/7/13.
//

#include "ShaderGenerator.h"

#include <regex>
#include <unordered_set>
#include <iostream>

#include <Common/Logger.h>
#include <IO/FileManager.h>

namespace Explosion {

    const std::string INCLUDE_MACRO = "#include";
    const char LEFT_CHR = '<';
    const char RIGHT_CHR = '>';
    std::string g_basicDir = "";

    using LoadedSet = std::unordered_set<std::string>;
    using SizeType = std::string::size_type;

    static std::pair<SizeType, SizeType> GetUrl(const std::string& str, SizeType offset)
    {
        auto left = str.find_first_of(LEFT_CHR, offset) + 1;
        auto right = str.find_first_of(RIGHT_CHR, left);
        return {left, right};
    }

    static std::string LoadShader(LoadedSet& set, const std::string& url)
    {
        std::cout << "load shader:" << url << std::endl;
        auto iter = set.emplace(url);
        if (!iter.second) {
            return "";
        }

        std::vector<char> file;
        file = IO::FileManager::ReadFile(url, false);
        std::string src(file.begin(), file.end());

        auto it = src.find(INCLUDE_MACRO);
        while (it != std::string::npos) {
            auto pair = GetUrl(src, it);
            if (pair.first != std::string::npos && pair.second != std::string::npos) {
                auto subUrl = src.substr(pair.first, pair.second - pair.first);
                src.replace(it, pair.second - it + 1, LoadShader(set, g_basicDir + subUrl));
            }

            it = src.find(INCLUDE_MACRO);
        }
        return src;
    }

    void ShaderGenerator::SetBasic(const std::string& dir)
    {
        g_basicDir = dir + "/";
    }

    std::string ShaderGenerator::Generate(const std::string& url)
    {
        LoadedSet set;
        return LoadShader(set, url);
    }
}