//
// Created by johnk on 2024/6/23.
//

#include <Editor/Core.h>
#include <Core/Cmdline.h>

namespace Editor {
    static ::Core::CmdlineArgValue<std::string> caRhiType(
        "rhiType", "-rhi", RHI::GetPlatformDefaultRHIAbbrString(),
        "rhi abbr string, can be 'dx12' or 'vulkan'");

    static ::Core::CmdlineArgValue<std::string> caProjectFile(
        "projectFile", "-project", "",
        "project file path");
}

namespace Editor {
    Core& Core::Get()
    {
        static Core instance;
        return instance;
    }

    Core::Core()
        : engine(nullptr)
    {
    }

    Core::~Core() = default;

    void Core::Initialize(int argc, char** argv)
    {
        ParseCommandLineArgs(argc, argv);
        InitializeRuntime();
    }

    void Core::Cleanup() // NOLINT
    {
        Runtime::EngineHolder::Unload();
        engine = nullptr;
        ::Core::ModuleManager::Get().Unload("Runtime");
    }

    Runtime::Engine* Core::GetEngine() const
    {
        return engine;
    }

    bool Core::ProjectHasSet() const // NOLINT
    {
        return !caProjectFile.GetValue().empty();
    }

    void Core::ParseCommandLineArgs(int argc, char** argv) const // NOLINT
    {
        ::Core::Cli::Get().Parse(argc, argv);
    }

    void Core::InitializeRuntime()
    {
        Runtime::EngineInitParams params {};
        params.logToFile = true;
        params.projectFile = caProjectFile.GetValue();
        params.rhiType = caRhiType.GetValue();

        Runtime::EngineHolder::Load("Editor", params);
        engine = &Runtime::EngineHolder::Get();
    }
}
