//
// Created by johnk on 2022/7/18.
//

#include <iostream>

#include <clipp.h>

#include <Engine/Engine.h>
#include <Common/Debug.h>

namespace Runtime {
    Engine& Engine::Get()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() = default;

    Engine::~Engine() = default;

    bool Engine::Initialize(int argc, char* argv[])
    {
        struct CommandLineParseResult {
            std::string exeFile;
            std::string projectFile;
            std::string rhiString;
        } result;

        // parse command line arguments
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
        }
        
        // initialize path mapper
        {
            std::unordered_map<std::string, std::string> pathMap;
#if BUILD_EDITOR
            pathMap["/Engine"] = Common::PathUtils::GetParentPath(result.exeFile) + "/Engine";
#else
            pathMap["/Engine"] = Common::PathUtils::GetParentPath(result.projectFile) + "/Engine";
#endif
            pathMap["/Game"] = Common::PathUtils::GetParentPath(result.exeFile) + "/Game";

            pathMapper = std::make_unique<Common::PathMapper>(Common::PathMapper::From(pathMap));
        }
        return true;
    }

    void Engine::Tick()
    {
        // TODO
    }

    const Common::PathMapper& Engine::GetPathMapper() const
    {
        Assert(pathMapper != nullptr);
        return *pathMapper;
    }
}
