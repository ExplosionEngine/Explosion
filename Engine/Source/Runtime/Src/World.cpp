//
// Created by johnk on 2023/9/5.
//

#include <Runtime/World.h>

namespace Runtime {
    World::World(std::string inName)
        : name(std::move(inName))
    {
    }

    World::~World() = default;

    void World::BroadcastSystemEvent(Mirror::TypeId eventTypeId, Mirror::Any eventRef)
    {
        // TODO
    }
}
