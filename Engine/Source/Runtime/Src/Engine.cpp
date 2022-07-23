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
        // TODO
    }

    void Engine::MainLoop()
    {
        // TODO
    }
}
