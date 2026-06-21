//
// Created by johnk on 2026/6/20.
//

#include <format>
#include <string_view>
#include <vector>

#include <Common/Template.h>
#include <Common/File.h>

namespace Common::Internal {
    // '@var@' placeholders mirror CMake's configure_file(@ONLY), so the same templates render identically here and
    // CMake-side.
    constexpr std::string_view openTag = "@";
    constexpr std::string_view closeTag = "@";

    static std::string Trim(const std::string& inStr)
    {
        constexpr std::string_view whitespace = " \t\r\n";
        const size_t begin = inStr.find_first_not_of(whitespace);
        if (begin == std::string::npos) {
            return {};
        }
        const size_t end = inStr.find_last_not_of(whitespace);
        return inStr.substr(begin, end - begin + 1);
    }

    static std::string FormatUnresolved(const std::vector<std::string>& inKeys)
    {
        std::string joined;
        for (size_t i = 0; i < inKeys.size(); i++) {
            joined += std::format("'{}'", inKeys[i]);
            if (i + 1 < inKeys.size()) {
                joined += ", ";
            }
        }
        return std::format("template has unresolved variable(s): {}", joined);
    }
}

namespace Common {
    TemplateEngine& TemplateEngine::Set(const std::string& inKey, const std::string& inValue)
    {
        variables[inKey] = inValue;
        return *this;
    }

    TemplateEngine& TemplateEngine::Set(const std::unordered_map<std::string, std::string>& inVariables)
    {
        for (const auto& [key, value] : inVariables) {
            variables[key] = value;
        }
        return *this;
    }

    bool TemplateEngine::Has(const std::string& inKey) const
    {
        return variables.contains(inKey);
    }

    void TemplateEngine::Clear()
    {
        variables.clear();
    }

    Result<std::string, std::string> TemplateEngine::Render(const std::string& inText) const
    {
        std::string result;
        result.reserve(inText.size());
        std::vector<std::string> unresolved;

        size_t cursor = 0;
        while (cursor < inText.size()) {
            const size_t open = inText.find(Internal::openTag, cursor);
            if (open == std::string::npos) {
                result.append(inText, cursor, std::string::npos);
                break;
            }
            result.append(inText, cursor, open - cursor);

            const size_t keyStart = open + Internal::openTag.size();
            const size_t close = inText.find(Internal::closeTag, keyStart);
            if (close == std::string::npos) {
                unresolved.emplace_back(Internal::Trim(inText.substr(keyStart)));
                break;
            }

            const std::string key = Internal::Trim(inText.substr(keyStart, close - keyStart));
            if (const auto it = variables.find(key);
                it != variables.end()) {
                result.append(it->second);
            } else {
                unresolved.emplace_back(key);
            }
            cursor = close + Internal::closeTag.size();
        }

        if (!unresolved.empty()) {
            return Err(Internal::FormatUnresolved(unresolved));
        }
        return Ok(std::move(result));
    }

    Result<std::string, std::string> TemplateEngine::RenderFile(const std::string& inSrcFile) const
    {
        return FileUtils::ReadTextFile(inSrcFile)
            .AndThen([this](const std::string& inText) { return Render(inText); });
    }

    Result<void, std::string> TemplateEngine::RenderFileTo(const std::string& inSrcFile, const std::string& inDstFile) const
    {
        return RenderFile(inSrcFile)
            .AndThen([&inDstFile](const std::string& inRendered) { return FileUtils::WriteTextFile(inDstFile, inRendered); });
    }
}
