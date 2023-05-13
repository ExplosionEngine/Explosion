//
// Created by johnk on 2022/5/2.
//

#pragma once

#include <string>
#include <locale>
#include <codecvt>
#include <regex>

#define FMT_HEADER_ONLY 1
#include <fmt/format.h>

namespace Common {
    class StringUtils {
    public:
        static inline std::wstring ToWideString(const std::string& src)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.from_bytes(src);
        }

        static inline std::string ToUpperCase(const std::string& src)
        {
            std::string result = src;
            std::transform(src.begin(), src.end(), result.begin(), [](const auto& c) { return std::toupper(c); });
            return result;
        }

        static inline std::string ToLowerCase(const std::string& src)
        {
            std::string result = src;
            std::transform(src.begin(), src.end(), result.begin(), [](const auto& c) { return std::tolower(c); });
            return result;
        }

        static inline std::string Replace(const std::string& src, const std::string& match, const std::string& replace)
        {
            std::string result = src;
            size_t pos;
            while ((pos = result.find(match)) != std::string::npos) {
                result.replace(pos, match.size(), replace);
            }
            return result;
        }

        static inline std::vector<std::string> Split(const std::string& src, const std::string& split)
        {
            std::vector<std::string> result;
            size_t pos;
            size_t searchStart = 0;
            while ((pos = src.find(split, searchStart)) != std::string::npos) {
                result.emplace_back(src.substr(searchStart, pos - searchStart));
                searchStart = pos + split.length();
            }
            if (searchStart < src.size()) {
                result.emplace_back(src.substr(searchStart, src.size() - searchStart));
            }
            return result;
        }

        static inline bool RegexMatch(const std::string& src, const std::string& regex)
        {
            std::regex expression(regex);
            return std::regex_match(src.begin(), src.end(), expression);
        }

        static inline std::string RegexSearchFirst(const std::string& src, const std::string& regex)
        {
            std::regex expression(regex);
            std::smatch match;
            return std::regex_search(src.begin(), src.end(), match, expression) ? match[0].str() : "";
        }

        static inline std::vector<std::string> RegexSearch(const std::string& src, const std::string& regex)
        {
            std::vector<std::string> result;
            std::regex expression(regex);
            std::smatch match;
            auto searchStart = src.begin();
            while (std::regex_search(searchStart, src.end(), match, expression)) {
                result.emplace_back(match[0].str());
                searchStart = match[0].second;
            }
            return result;
        }
    };
}
