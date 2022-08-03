//
// Created by johnk on 2022/8/3.
//

#include <Engine/Engine.h>

namespace Engine {
    Engine& Engine::Get()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() = default;

    Engine::~Engine() = default;

    void Engine::Initialize(int argc, char** argv)
    {
        // TODO
    }

    void Engine::Tick()
    {
        // TODO
    }

    InputManager& Engine::GetInputManager()
    {
        return inputManager;
    }
}
