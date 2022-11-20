//
// Created by johnk on 2022/11/24.
//

#pragma once

#include <fstream>

#include <Common/Utility.h>

namespace MirrorTool {
    class Generator {
    public:
        using Result = std::pair<bool, std::string>;

        NON_COPYABLE(Generator)
        explicit Generator(std::string inOutputFile);
        ~Generator();

        Result Generate();

    private:
        std::string outputFile;
        std::ofstream file;
    };
}
