//
// Created by johnk on 2024/4/14.
//

#include <fstream>
#include <cstdio>
#include <format>

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/error.h>

#include <Common/File.h>
#include <Common/FileSystem.h>

namespace Common {
    Result<std::string, std::string> FileUtils::ReadTextFile(const std::string& inFileName)
    {
        std::ifstream file(inFileName, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            return Err(std::format("failed to open file '{}' for reading", inFileName));
        }

        const size_t size = file.tellg();
        std::string result;
        result.resize(size);
        file.seekg(0);
        file.read(result.data(), static_cast<std::streamsize>(size));
        file.close();
        return Ok(std::move(result));
    }

    Result<void, std::string> FileUtils::WriteTextFile(const std::string& inFileName, const std::string& inContent)
    {
        const Path path(inFileName);
        if (const Path parentPath = path.Parent();
            !parentPath.Exists()) {
            parentPath.MakeDir();
        }

        std::ofstream file(inFileName, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return Err(std::format("failed to open file '{}' for writing", inFileName));
        }
        file.write(inContent.data(), static_cast<std::streamsize>(inContent.size()));
        file.close();
        return Ok();
    }

    Result<rapidjson::Document, std::string> FileUtils::ReadJsonFile(const std::string& inFileName)
    {
        std::FILE* file = fopen(inFileName.c_str(), "rb"); // NOLINT
        if (file == nullptr) {
            return Err(std::format("failed to open json file '{}' for reading", inFileName));
        }

        char buffer[65536];
        rapidjson::FileReadStream stream(file, buffer, sizeof(buffer));
        rapidjson::Document document;
        document.ParseStream(stream);
        (void) fclose(file);

        if (document.HasParseError()) {
            return Err(std::format("failed to parse json file '{}' (error code {} at offset {})",
                inFileName, static_cast<int>(document.GetParseError()), document.GetErrorOffset()));
        }
        return Ok(std::move(document));
    }

    Result<void, std::string> FileUtils::WriteJsonFile(const std::string& inFileName, const rapidjson::Document& inJsonDocument, bool inPretty)
    {
        if (Path parentPath = Path(inFileName).Parent();
            !parentPath.Exists()) {
            parentPath.MakeDir();
        }

        std::FILE* file = fopen(inFileName.c_str(), "wb"); // NOLINT
        if (file == nullptr) {
            return Err(std::format("failed to open json file '{}' for writing", inFileName));
        }

        char buffer[65536];
        rapidjson::FileWriteStream stream(file, buffer, sizeof(buffer));
        if (inPretty) {
            rapidjson::PrettyWriter writer(stream);
            inJsonDocument.Accept(writer);
        } else {
            rapidjson::Writer writer(stream);
            inJsonDocument.Accept(writer);
        }
        (void) fclose(file);
        return Ok();
    }
}
