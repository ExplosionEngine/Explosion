//
// Created by johnk on 2022/8/1.
//

#include <iostream>

#include <clipp.h>
#include <GLFW/glfw3.h>

#include <Common/Path.h>
#include <Engine/Engine.h>

struct CommandLineParseResult {
    std::string exeFile;
    std::string projectFile;
    std::string rhiString;
};

static bool ParseCommandLineArguments(CommandLineParseResult& result, int argc, char* argv[])
{
    result.exeFile = argv[0];

    auto cli = (
        clipp::value("project file", result.projectFile),
        clipp::required("-rhi").doc("RHI type, can be 'DirectX12' or 'Vulkan'") & clipp::value("RHI type", result.rhiString)
    );

    if (!clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return false;
    }
    return true;
}

static Runtime::EngineInitializer PopulateEngineInitializer(const CommandLineParseResult& result)
{
    Runtime::EngineInitializer initializer;
#if BUILD_EDITOR
    initializer.pathMap["/Engine"] = Common::PathUtils::GetParentPath(result.exeFile) + "/Engine";
#else
    initializer.pathMap["/Engine"] = Common::PathUtils::GetParentPath(result.projectFile) + "/Engine";
#endif
    initializer.pathMap["/Game"] = Common::PathUtils::GetParentPath(result.exeFile) + "/Game";
    return initializer;
}

int main(int argc, char* argv[])
{
    CommandLineParseResult result;
    if (!ParseCommandLineArguments(result, argc, argv)) {
        return 1;
    }
    {
        Runtime::EngineInitializer initializer = PopulateEngineInitializer(result);
        Runtime::Engine::Get().Initialize(initializer);
    }
    // TODO glfw main loop & engine tick
    return 0;
}
