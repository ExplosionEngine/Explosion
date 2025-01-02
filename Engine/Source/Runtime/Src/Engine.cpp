//
// Created by johnk on 2024/8/21.
//

#include <Runtime/Engine.h>
#include <Common/Debug.h>
#include <Core/Module.h>
#include <Core/Paths.h>
#include <Runtime/World.h>

namespace Runtime {
    Engine::Engine(const EngineInitParams& inParams)
    {
        if (!inParams.projectFile.empty()) {
            Core::Paths::SetCurrentProjectFile(inParams.projectFile);
        }

        renderModule = ::Core::ModuleManager::Get().FindOrLoadTyped<Render::RenderModule>("Render");
        Assert(renderModule != nullptr);

        Render::RenderModuleInitParams initParams;
        initParams.rhiType = RHI::GetRHITypeByAbbrString(inParams.rhiType);
        renderModule->Initialize(initParams);
    }

    Engine::~Engine()
    {
        renderModule->DeInitialize();
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

    void Engine::Tick(float inTimeSeconds) const
    {
        for (auto* world : worlds) {
            if (!world->Playing()) {
                continue;
            }
            world->Tick(inTimeSeconds);
        }
    }

    Common::UniqueRef<World> Engine::CreateWorld(const std::string& inName) const // NOLINT
    {
        return new World(inName);
    }

    Common::UniqueRef<Engine> EngineHolder::engine = nullptr;

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
        auto& gameModule = Core::ModuleManager::Get().GetOrLoadTyped<IEngineModule>(inModuleName);
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
