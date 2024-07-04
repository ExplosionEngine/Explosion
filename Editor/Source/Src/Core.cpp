//
// Created by johnk on 2024/6/23.
//

#include <Editor/Core.h>
#include <Core/Cmdline.h>

Core::CmdlineArgValue<std::string> caRhiType(
    "rhiType", "-rhi", RHI::GetPlatformDefaultRHIAbbrString(),
    "rhi abbr string, can be 'dx12' or 'vulkan'");

Core::CmdlineArgValue<std::string> caProjectRoot(
    "projectRoot", "-project", "",
    "project root path");

namespace Editor {
    Core& Core::Get()
    {
        static Core instance;
        return instance;
    }

    Core::Core()
        : runtimeModule(nullptr)
        , renderingModule(nullptr)
        , engine(nullptr)
    {
    }

    Core::~Core() = default;

    void Core::Initialize(int argc, char** argv)
    {
        ParseCommandLineArgs(argc, argv);
        InitializeRuntime();
        InitializeRendering();
    }

    void Core::Cleanup() // NOLINT
    {
        ::Core::ModuleManager::Get().Unload("Runtime");
        ::Core::ModuleManager::Get().Unload("Rendering");
    }

    Runtime::EditorEngine* Core::GetEngine() const
    {
        return engine;
    }

    bool Core::ProjectRooHasSet() const // NOLINT
    {
        return !caProjectRoot.GetValue().empty();
    }

    Runtime::RuntimeModule* Core::GetRuntimeModule() const
    {
        return runtimeModule;
    }

    Rendering::RenderingModule* Core::GetRenderingModule() const
    {
        return renderingModule;
    }

    void Core::ParseCommandLineArgs(int argc, char** argv) const // NOLINT
    {
        ::Core::Cli::Get().Parse(argc, argv);
    }

    void Core::InitializeRuntime()
    {
        runtimeModule = ::Core::ModuleManager::Get().FindOrLoadTyped<Runtime::RuntimeModule>("Runtime");
        Assert(runtimeModule != nullptr);

        Runtime::RuntimeModuleInitParams initParams;
        initParams.isEditor = true;
        runtimeModule->Initialize(initParams);

        engine = static_cast<Runtime::EditorEngine*>(runtimeModule->GetEngine());
    }

    void Core::InitializeRendering()
    {
        renderingModule = ::Core::ModuleManager::Get().FindOrLoadTyped<Rendering::RenderingModule>("Rendering");
        Assert(renderingModule != nullptr);

        Rendering::RenderingModuleInitParams initParams;
        initParams.rhiType = RHI::RHIAbbrStringToRHIType(caRhiType.GetValue());
        renderingModule->Initialize(initParams);
    }
}
