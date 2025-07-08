//
// Created by johnk on 2024/4/14.
//

#include <fstream>
#include <cstdio>

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include <Common/File.h>
#include <Common/Debug.h>
#include <Common/FileSystem.h>

namespace Common {
    std::string FileUtils::ReadTextFile(const std::string& inFileName)
    {
        std::string result;
        {
            std::ifstream file(inFileName, std::ios::ate | std::ios::binary);
            Assert(file.is_open());
            const size_t size = file.tellg();
            result.resize(size);
            file.seekg(0);
            file.read(result.data(), static_cast<std::streamsize>(size));
            file.close();
        }
        return result;
    }

    void FileUtils::WriteTextFile(const std::string& inFileName, const std::string& inContent)
    {
        const Path path(inFileName);
        if (const Path parentPath = path.Parent();
            !parentPath.Exists()) {
            parentPath.MakeDir();
        }

        std::ofstream file(inFileName, std::ios::out | std::ios::binary | std::ios::trunc);
        Assert(file.is_open());
        file.write(inContent.data(), static_cast<std::streamsize>(inContent.size()));
        file.close();
    }

    rapidjson::Document FileUtils::ReadJsonFile(const std::string& inFileName)
    {
        char buffer[65536];
        std::FILE* file = fopen(inFileName.c_str(), "rb"); // NOLINT
        rapidjson::FileReadStream stream(file, buffer, sizeof(buffer));

        rapidjson::Document document;
        document.ParseStream(stream);
        (void) fclose(file);
        return document;
    }

    void FileUtils::WriteJsonFile(const std::string& inFileName, const rapidjson::Document& inJsonDocument, bool inPretty)
    {
        Common::Path parentPath = Common::Path(inFileName).Parent();
        if (!parentPath.Exists()) {
            parentPath.MakeDir();
        }

        char buffer[65536];
        std::FILE* file = fopen(inFileName.c_str(), "wb"); // NOLINT
        rapidjson::FileWriteStream stream(file, buffer, sizeof(buffer));

        if (inPretty) {
            rapidjson::PrettyWriter writer(stream);
            inJsonDocument.Accept(writer);
        } else {
            rapidjson::Writer writer(stream);
            inJsonDocument.Accept(writer);
        }
        (void) fclose(file);
    }
}
