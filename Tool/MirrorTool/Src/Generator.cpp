//
// Created by johnk on 2022/11/24.
//

#include <MirrorTool/Generator.h>

namespace MirrorTool {
    Generator::Generator(std::string inOutputFile, std::string inHeaderDir, const MetaInfo& inMetaInfo)
        : outputFile(std::move(inOutputFile)), headerDir(std::move(inHeaderDir)), metaInfo(inMetaInfo) {}

    Generator::~Generator() = default;

    Generator::Result Generator::Generate()
    {
        // TODO
        return {};
    }
}
