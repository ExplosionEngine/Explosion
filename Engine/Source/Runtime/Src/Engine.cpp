//
// Created by johnk on 2024/8/21.
//

#include <Runtime/Engine.h>
#include <Core/Module.h>
#include <Common/Debug.h>
#include <Core/Paths.h>

namespace Runtime {
    Engine::Engine(const EngineInitParams& inParams)
    {
        if (!inParams.projectFile.empty()) {
            Core::Paths::SetCurrentProjectFile(inParams.projectFile);
        }

        renderModule = ::Core::ModuleManager::Get().FindOrLoadTyped<Render::RenderModule>("Render");
        Assert(renderModule != nullptr);

        Render::RenderModuleInitParams initParams;
        initParams.rhiType = RHI::RHIAbbrStringToRHIType(inParams.rhiType);
        renderModule->Initialize(initParams);
    }

    Engine::~Engine()
    {
        ::Core::ModuleManager::Get().Unload("Render");
    }

    void Engine::MountWorld(World* inWorld)
    {
        worlds.emplace(inWorld);
    }

    void Engine::UnmountWorld(World* inWorld)
    {
        worlds.erase(inWorld);
    }

    Render::RenderModule& Engine::GetRenderModule() const
    {
        return *renderModule;
    }

    void Engine::Tick(float inTimeMs) const
    {
        // TODO tick each playing world
        // TODO fetch each scene
        // TODO traverse each view in scene, create a renderer, perform rendering
    }

    Engine* EngineHolder::engine = nullptr;

    MinEngine::MinEngine(const EngineInitParams& inParams)
        : Engine(inParams)
    {
    }

    MinEngine::~MinEngine() = default;

    bool MinEngine::IsEditor()
    {
        return false;
    }

    void EngineHolder::Load(const std::string& inModuleName, const EngineInitParams& inInitParams)
    {
        Assert(engine == nullptr);
        auto& gameModule = Core::ModuleManager::Get().GetOrLoadTyped<IGameModule>(inModuleName);
        engine = gameModule.CreateEngine(inInitParams);
    }

    void EngineHolder::Unload()
    {
        Assert(engine != nullptr);
        engine = nullptr;
    }

    Engine& EngineHolder::Get()
    {
        AssertWithReason(engine != nullptr, "no valid engine");
        return *engine;
    }
}
