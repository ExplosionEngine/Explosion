//
// Created by johnk on 2022/11/20.
//

#include <clipp.h>

#include <iostream>

#include <MirrorTool/Parser.h>
#include <MirrorTool/Generator.h>

int main(int argc, char* argv[])
{
    std::string inputFile;
    std::string outputFile;

    auto cli = (
        clipp::required("-i").doc("input header file") & clipp::value("input header file", inputFile),
        clipp::required("-o").doc("output file") & clipp::value("output file", outputFile)
    );
    if (!clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return 1;
    }

    if (!inputFile.ends_with(".h")) {
        std::cout << "input header file must ends with .h" << std::endl;
        return 1;
    }
    if (!outputFile.ends_with(".cpp")) {
        std::cout << "output file must ends with .cpp" << std::endl;
        return 1;
    }

    MirrorTool::Parser parser(inputFile);
    auto metaInfo = parser.Parse();

    if (!metaInfo.first) {
        std::cout << std::get<std::string>(metaInfo.second) << std::endl;
        return 1;
    }

    MirrorTool::Generator generator(outputFile, metaInfo);
    auto result = generator.Generate();

    if (!result.first) {
        std::cout << result.second << std::endl;
        return 1;
    }
    return 0;
}
