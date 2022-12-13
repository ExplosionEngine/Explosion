//
// Created by johnk on 2022/11/24.
//

#pragma once

#include <fstream>

#include <Common/Utility.h>

namespace MirrorTool {
    struct MetaInfo;

    class Generator {
    public:
        using Result = std::pair<bool, std::string>;

        NON_COPYABLE(Generator)
        explicit Generator(std::string inOutputFile, std::string inHeaderDir, const MetaInfo& inMetaInfo);
        ~Generator();

        Result Generate();

    private:
        const MetaInfo& metaInfo;
        std::string outputFile;
        std::string headerDir;
        std::ofstream file;
    };
}
