//
// Created by johnk on 2022/11/24.
//

#include <MirrorTool/Generator.h>

namespace MirrorTool {
    Generator::Generator(std::string inOutputFile) : outputFile(std::move(inOutputFile)) {}

    Generator::~Generator() = default;

    Generator::Result Generator::Generate()
    {
        // TODO
        return {};
    }
}
