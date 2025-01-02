//
// Created by johnk on 2022/11/24.
//

#pragma once

#include <fstream>

#include <Common/Utility.h>

#include <MirrorTool/Parser.h>

namespace MirrorTool {
    struct MetaInfo;

    class Generator {
    public:
        using Result = std::pair<bool, std::string>;

        NonCopyable(Generator)
        explicit Generator(std::string inInputFile, std::string inOutputFile, std::vector<std::string> inHeaderDirs, const MetaInfo& inMetaInfo);
        ~Generator();

        Result Generate() const;

    private:
        Result GenerateCode(std::ifstream& inFile, std::ofstream& outFile, size_t uniqueId) const;

        const MetaInfo& metaInfo;
        std::string inputFile;
        std::string outputFile;
        std::vector<std::string> headerDirs;
    };
}
