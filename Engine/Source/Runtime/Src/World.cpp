//
// Created by johnk on 2024/10/31.
//

#include <Runtime/World.h>
#include <Runtime/Engine.h>

namespace Runtime {
    World::World(const std::string& inName)
        : name(inName)
    {
        EngineHolder::Get().MountWorld(this);
    }

    World::~World()
    {
        EngineHolder::Get().UnmountWorld(this);
    }
} // namespace Runtime
