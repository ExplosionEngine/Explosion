//
// Created by johnk on 2024/8/2.
//

#include <Runtime/World.h>

namespace Runtime {
    Commands::Commands(World& inWorld)
        : world(inWorld)
    {
    }

    Commands::~Commands() = default;

    Entity Commands::CreateEntity() // NOLINT
    {
        return world.registry.create();
    }

    void Commands::DestroyEntity(Entity inEntity) // NOLINT
    {
        world.registry.destroy(inEntity);
    }

    World::World(std::string inName)
        : name(std::move(inName))
    {
    }

    World::~World() = default;

    void World::Duplicate(World& inWorld) const
    {
        // TODO
    }
}
