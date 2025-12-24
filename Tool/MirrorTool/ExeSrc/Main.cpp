//
// Created by johnk on 2022/11/20.
//

#include <sstream>

#include <clipp.h>

#include <MirrorTool/Parser.h>
#include <MirrorTool/Generator.h>
#include <Common/IO.h>
#include <Common/String.h>
#include <Common/FileSystem.h>

#define OUTPUT_FULL_CMDLINE 0

static std::vector<std::string> ProcessHeaderDirs(const std::vector<std::string>& headerDirs)
{
    std::unordered_set<std::string> duplicated;
    std::vector<std::string> result;
    for (const auto& headerDir : headerDirs) {
        for (auto splitHeaderDirs = Common::StringUtils::Split(headerDir, ";");
            const auto& splitHeaderDir : splitHeaderDirs) {
            if (duplicated.contains(splitHeaderDir)) {
                continue;
            }
            result.emplace_back(splitHeaderDir);
            duplicated.emplace(splitHeaderDir);
        }
    }
    return result;
}

int main(int argc, char* argv[]) // NOLINT
{
    AutoCoutFlush;

    std::stringstream fullCmdline;
    for (auto i = 0; i < argc; i++) {
        fullCmdline << argv[i];
        if (i != argc - 1) {
            fullCmdline << " ";
        }
    }
    const std::string fullCmdLineStr = fullCmdline.str();

    std::string inputFile;
    std::string outputFile;
    std::vector<std::string> headerDirs;
    std::vector<std::string> frameworkDirs;
    bool dynamic = false;

    if (const auto cli = (
            clipp::required("-i").doc("input header file") & clipp::value("input header file", inputFile),
            clipp::required("-o").doc("output file") & clipp::value("output file", outputFile),
            clipp::option("-I").doc("header search dirs") & clipp::values("header search dirs", headerDirs),
            clipp::option("-F").doc("framework search dirs") & clipp::values("framework search dirs", frameworkDirs),
            clipp::option("-d").set(dynamic).doc("used for dynamic library (auto unload some metas)"));
        !clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return 1;
    }

    inputFile = Common::Path(inputFile).String();
    outputFile = Common::Path(outputFile).String();
    for (auto& headerDir : headerDirs) {
        headerDir = Common::Path(headerDir).String();
    }
    headerDirs = ProcessHeaderDirs(headerDirs);

    auto outputErrorWithDebugContext = [fullCmdLineStr, inputFile, outputFile, headerDirs, frameworkDirs, dynamic](const std::string& error) -> void {
        std::cout << "MirrorTool fatal error:" << Common::newline;
        std::cout << error << Common::newline;
        std::cout << "MirrorTool debug context: " << Common::newline;
        std::cout << "[fullCmdLine] " << fullCmdLineStr << Common::newline;
        std::cout << "[dynamic] " << dynamic << Common::newline;
        std::cout << "[inputFile] " << inputFile << Common::newline;
        std::cout << "[outputFile] " << outputFile << Common::newline;
        std::cout << "[headerDirs]" << Common::newline;
        for (const auto& headerDir : headerDirs) {
            std::cout << headerDir << Common::newline;
        }
        std::cout << "[frameworkDirs]" << Common::newline;
        for (const auto& frameworkDir : frameworkDirs) {
            std::cout << frameworkDir << Common::newline;
        }
        std::cout << Common::newline;
    };

    if (!inputFile.ends_with(".h")) {
        outputErrorWithDebugContext("input header file must ends with .h");
        return 1;
    }
    if (!outputFile.ends_with(".cpp")) {
        outputErrorWithDebugContext("output file must ends with .cpp");
        return 1;
    }

    MirrorTool::Parser parser(inputFile, headerDirs, frameworkDirs);
    auto [parseSuccess, parseResultOrError] = parser.Parse();
    if (!parseSuccess) {
        outputErrorWithDebugContext(std::get<std::string>(parseResultOrError));
        return 1;
    }

    MirrorTool::Generator generator(inputFile, outputFile, headerDirs, std::get<MirrorTool::MetaInfo>(parseResultOrError), dynamic);
    if (auto [generateSuccess, generateError] = generator.Generate();
        !generateSuccess) {
        outputErrorWithDebugContext(generateError);
        return 1;
    }
    return 0;
}
