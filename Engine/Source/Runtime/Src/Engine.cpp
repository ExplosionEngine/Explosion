//
// Created by johnk on 2024/4/7.
//

#include <Runtime/Engine.h>

namespace Runtime {
    Engine::Engine(const Runtime::EngineInitParams& inParams)
    {
    }

    Engine::~Engine() = default;

    GameEngine::GameEngine(const EngineInitParams& inParams)
        : Engine(inParams)
    {
    }

    GameEngine::~GameEngine() = default;
    
}
