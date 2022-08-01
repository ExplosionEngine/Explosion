//
// Created by johnk on 2022/7/18.
//

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

    void Engine::Initialize(const EngineInitializer& initializer)
    {
        pathMapper = std::make_unique<Common::PathMapper>(std::move(Common::PathMapper::From(initializer.pathMap)));
    }

    void Engine::MainLoop()
    {
        // TODO
    }

    const Common::PathMapper& Engine::GetPathMapper() const
    {
        Assert(pathMapper != nullptr);
        return *pathMapper;
    }
}
