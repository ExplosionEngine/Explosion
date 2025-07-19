//
// Created by johnk on 2022/7/25.
//

#pragma once

#include <string>

#include <rapidjson/document.h>

namespace Common {
    class FileUtils {
    public:
        static std::string ReadTextFile(const std::string& inFileName);
        static void WriteTextFile(const std::string& inFileName, const std::string& inContent);
        static rapidjson::Document ReadJsonFile(const std::string& inFileName);
        static void WriteJsonFile(const std::string& inFileName, const rapidjson::Document& inJsonDocument, bool inPretty = true);
    };
}
