//
// Created by Administrator on 2021/4/2 0002.
//

#include <Explosion/Core/World.h>

#include <utility>

namespace Explosion {
    World::World(std::string name) : name(std::move(name)) {}

    World::~World() = default;

    Entity World::CreateEntity()
    {
        return registry.create();
    }

    void World::DestroyEntity(const Entity& entity)
    {
        registry.destroy(entity);
    }
}
