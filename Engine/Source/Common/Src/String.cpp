//
// Created by johnk on 2024/4/14.
//

#include <locale>
#include <codecvt>
#include <regex>

#include <Common/String.h>

namespace Common {
    std::wstring StringUtils::ToWideString(const std::string& src)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter; // NOLINT
        return converter.from_bytes(src);
    }

    std::string StringUtils::ToByteString(const std::wstring& src)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter; // NOLINT
        return converter.to_bytes(src);
    }

    std::string StringUtils::ToUpperCase(const std::string& src)
    {
        std::string result = src;
        std::ranges::transform(src.begin(), src.end(), result.begin(), [](const auto& c) { return std::toupper(c); });
        return result;
    }

    std::string StringUtils::ToLowerCase(const std::string& src)
    {
        std::string result = src;
        std::ranges::transform(src.begin(), src.end(), result.begin(), [](const auto& c) { return std::tolower(c); });
        return result;
    }

    std::string StringUtils::Replace(const std::string& src, const std::string& match, const std::string& replace)
    {
        std::string result = src;
        size_t pos;
        while ((pos = result.find(match)) != std::string::npos) {
            result.replace(pos, match.size(), replace);
        }
        return result;
    }

    std::vector<std::string> StringUtils::Split(const std::string& src, const std::string& split)
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

    bool StringUtils::RegexMatch(const std::string& src, const std::string& regex)
    {
        const std::regex expression(regex);
        return std::regex_match(src.begin(), src.end(), expression);
    }

    std::string StringUtils::RegexSearchFirst(const std::string& src, const std::string& regex)
    {
        const std::regex expression(regex);
        std::smatch match;
        return std::regex_search(src.begin(), src.end(), match, expression) ? match[0].str() : "";
    }

    std::vector<std::string> StringUtils::RegexSearch(const std::string& src, const std::string& regex)
    {
        std::vector<std::string> result;
        const std::regex expression(regex);
        std::smatch match;
        auto searchStart = src.begin();
        while (std::regex_search(searchStart, src.end(), match, expression)) {
            result.emplace_back(match[0].str());
            searchStart = match[0].second;
        }
        return result;
    }

    std::string StringUtils::AfterFirst(const std::string& src, const std::string& split)
    {
        const size_t pos = src.find(split);
        if (pos == std::string::npos) {
            return "";
        }
        return src.substr(pos + split.length());
    }

    std::string StringUtils::BeforeFirst(const std::string& src, const std::string& split)
    {
        const size_t pos = src.find(split);
        if (pos == std::string::npos) {
            return "";
        }
        return src.substr(0, pos);
    }

    std::string StringUtils::AfterLast(const std::string& src, const std::string& split)
    {
        const size_t pos = src.find_last_of(split);
        if (pos == std::string::npos) {
            return "";
        }
        return src.substr(pos + split.length() - 1);
    }

    std::string StringUtils::BeforeLast(const std::string& src, const std::string& split)
    {
        const size_t pos = src.find_last_of(split);
        if (pos == std::string::npos) {
            return "";
        }
        return src.substr(0, pos - 1);
    }
}
