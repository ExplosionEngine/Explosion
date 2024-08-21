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
    }

    Engine::~Engine() = default;

    void Engine::MountWorld(World* inWorld)
    {
        worlds.emplace(inWorld);
    }

    void Engine::UnmountWorld(World* inWorld)
    {
        worlds.erase(inWorld);
    }

    Engine* EngineHolder::engine = nullptr;

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
