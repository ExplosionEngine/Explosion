//
// Created by johnk on 2026/6/20.
//

#pragma once

#include <string>
#include <unordered_map>

#include <Common/Result.h>

namespace Common {
    class TemplateEngine {
    public:
        TemplateEngine& Set(const std::string& inKey, const std::string& inValue);
        TemplateEngine& Set(const std::unordered_map<std::string, std::string>& inVariables);

        bool Has(const std::string& inKey) const;
        void Clear();

        Result<std::string, std::string> Render(const std::string& inText) const;
        Result<std::string, std::string> RenderFile(const std::string& inSrcFile) const;
        Result<void, std::string> RenderFileTo(const std::string& inSrcFile, const std::string& inDstFile) const;

    private:
        std::unordered_map<std::string, std::string> variables;
    };
}
