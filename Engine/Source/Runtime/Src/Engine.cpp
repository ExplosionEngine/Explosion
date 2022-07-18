//
// Created by johnk on 2022/7/18.
//

#include <Engine/Engine.h>

namespace Runtime {
    EngineInitializer EngineInitializer::FromCommandline(int argc, char* argv)
    {
        // TODO
        return {};
    }

    Engine& Engine::Get()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() = default;

    Engine::~Engine() = default;

    void Engine::Initialize(const EngineInitializer& initializer)
    {
        engineRoot = initializer.engineRoot;
        gameRoot = initializer.gameRoot;
    }

    void Engine::MainLoop()
    {
        // TODO
    }

    const std::string& Engine::EngineRoot() const
    {
        return engineRoot;
    }

    const std::string& Engine::GameRoot() const
    {
        return gameRoot;
    }
}
