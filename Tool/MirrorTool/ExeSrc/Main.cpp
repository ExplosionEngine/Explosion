//
// Created by johnk on 2022/11/20.
//

#include <clipp.h>
#include <filesystem>

#include <MirrorTool/Parser.h>
#include <MirrorTool/Generator.h>
#include <Common/IO.h>
#include <Common/String.h>

static std::string GetUnixStylePath(const std::string& inPath)
{
    return Common::StringUtils::Replace(std::filesystem::canonical(inPath).string(), "\\", "/");
}

int main(int argc, char* argv[]) // NOLINT
{
    AutoCoutFlush;

    std::string inputFile;
    std::string outputFile;
    std::vector<std::string> headerDirs;

    if (const auto cli = (
            clipp::required("-i").doc("input header file") & clipp::value("input header file", inputFile),
            clipp::required("-o").doc("output file") & clipp::value("output file", outputFile),
            clipp::option("-I").doc("header search dirs") & clipp::values("header search dirs", headerDirs));
        !clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return 1;
    }

    inputFile = GetUnixStylePath(inputFile);
    outputFile = GetUnixStylePath(outputFile);
    for (auto& headerDir : headerDirs) {
        headerDir = GetUnixStylePath(headerDir);
    }

    if (!inputFile.ends_with(".h")) {
        std::cout << "input header file must ends with .h" << Common::newline;
        return 1;
    }
    if (!outputFile.ends_with(".cpp")) {
        std::cout << "output file must ends with .cpp" << Common::newline;
        return 1;
    }

    MirrorTool::Parser parser(inputFile, headerDirs);
    auto [parseSuccess, parseResultOrError] = parser.Parse();

    if (!parseSuccess) {
        std::cout << std::get<std::string>(parseResultOrError) << Common::newline;
        return 1;
    }

    MirrorTool::Generator generator(inputFile, outputFile, headerDirs, std::get<MirrorTool::MetaInfo>(parseResultOrError));
    if (auto [generateSuccess, generateError] = generator.Generate();
        !generateSuccess) {
        std::cout << generateError << Common::newline;
        return 1;
    }
    return 0;
}
