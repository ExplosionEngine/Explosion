//
// Created by John Kindem on 2021/6/3.
//

#include <Engine/Engine.h>
#include <Engine/World.h>

namespace Explosion {
    Engine* Engine::GetInstance()
    {
        static auto* instance = new Engine();
        return instance;
    }

    World* Engine::CreateWorld()
    {
        return new World();
    }

    void Engine::DestroyWorld(World* world)
    {
        delete world;
    }
}
