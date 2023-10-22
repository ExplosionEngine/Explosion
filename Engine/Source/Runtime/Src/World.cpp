//
// Created by johnk on 2023/9/5.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/World.h>

namespace Runtime {
    World::World(std::string inName)
        : ECSHost()
        , name(std::move(inName))
    {
    }

    World::~World() = default;

    void World::Setup()
    {
        ECSHost::Setup();
    }

    void World::Tick(float timeMS)
    {
        ECSHost::Tick(timeMS);
    }

    void World::Shutdown()
    {
        ECSHost::Shutdown();
    }
}
