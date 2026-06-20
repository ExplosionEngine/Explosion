//
// Created by johnk on 2022/7/25.
//

#pragma once

#include <string>

#include <rapidjson/document.h>

#include <Common/Result.h>

namespace Common {
    class FileUtils {
    public:
        static Result<std::string, std::string> ReadTextFile(const std::string& inFileName);
        static Result<void, std::string> WriteTextFile(const std::string& inFileName, const std::string& inContent);
        static Result<rapidjson::Document, std::string> ReadJsonFile(const std::string& inFileName);
        static Result<void, std::string> WriteJsonFile(const std::string& inFileName, const rapidjson::Document& inJsonDocument, bool inPretty = true);
    };
}
