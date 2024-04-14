//
// Created by johnk on 2022/5/2.
//

#pragma once

#include <string>
#include <vector>

#define FMT_HEADER_ONLY 1
#include <fmt/format.h>

namespace Common {
    class StringUtils {
    public:
        static std::wstring ToWideString(const std::string& src);
        static std::string ToUpperCase(const std::string& src);
        static std::string ToLowerCase(const std::string& src);
        static std::string Replace(const std::string& src, const std::string& match, const std::string& replace);
        static std::vector<std::string> Split(const std::string& src, const std::string& split);
        static bool RegexMatch(const std::string& src, const std::string& regex);
        static std::string RegexSearchFirst(const std::string& src, const std::string& regex);
        static std::vector<std::string> RegexSearch(const std::string& src, const std::string& regex);
        static std::string AfterFirst(const std::string& src, const std::string& split);
        static std::string BeforeFirst(const std::string& src, const std::string& split);
        static std::string AfterLast(const std::string& src, const std::string& split);
        static std::string BeforeLast(const std::string& src, const std::string& split);
    };
}
